#include "planificacion.h"
#include "app.h"

t_list* cola_NEW;
t_list* cola_READY;
t_list* cola_BLOCKED;
t_list* cola_EXEC;
t_list* cola_EXIT;

t_dictionary_int* diccionario_colas;

t_config* config;


void planificar_corto_plazo()
{
	while (grado_multiprocesamiento > list_size(cola_EXEC)&&(!list_is_empty(cola_READY)))
		cambiar_estado_a(list_remove(cola_READY, 0), EXEC);  //meter en cola EXEC (todavia no ejecuta)


}



static void actualizar_estado_bloqueados()
{
	for (int i=0; i < list_size(cola_BLOCKED); i++)
	{
		t_pedido* pedido = list_get(cola_BLOCKED, i);
		if (pedido->instruccion_a_realizar == ESPERAR_EN_RESTAURANTE)
		{
			if (!pedido->esta_listo)
				continue;


			cambiar_estado_a(pedido, READY);
			pedido->instruccion_a_realizar = IR_A_CLIENTE;
		} else {
			//esta parte creo que tiene algun bug
			descansar_repartidor(pedido);
			if (pedido->ciclos_descansados >= pedido->repartidor->tiempo_descanso)
				cambiar_estado_a(pedido, READY);
		}

	}
}


static void actualizar_estado_ejecutados()
{
	for (int i=0; i < list_size(cola_EXEC); i++)
	{
		t_pedido* pedido = list_get(cola_EXEC, i);
		if (pedido->instruccion_a_realizar == IR_A_CLIENTE && misma_posicion(pedido->repartidor->posicion, pedido->posicion_cliente))
		{
			log_info(logger_app, "el pedido %d entrego el pedido al cliente", pedido->id_pedido);
			cambiar_estado_a(pedido, EXIT);
			list_add(lista_repartidores_libres, pedido->repartidor);
			pedido->repartidor = NULL;

		}
				else	{
			if (misma_posicion(pedido->repartidor->posicion, pedido->posicion_de_restaurante)) 	{
				if (pedido->esta_listo)
					pedido->instruccion_a_realizar = IR_A_CLIENTE;
				else {
					pedido->ciclos_ejecutandose = 0;
					cambiar_estado_a(pedido, BLOCKED);
					pedido->instruccion_a_realizar = ESPERAR_EN_RESTAURANTE;
				}
				}
			if (pedido->ciclos_ejecutandose == pedido->repartidor->frecuencia_descanso)	{
				pedido->ciclos_ejecutandose = 0;
				cambiar_estado_a(pedido, BLOCKED);
			}
		}
	}
}

void ejecutar_ciclo()
{

	for (int i=0; i < list_size(cola_EXEC); i++)
	{
		t_pedido* pedido = list_get(cola_EXEC, i);
		sem_post (&(pedido->mutex));
	}
	for (int i=0; i < list_size(cola_EXEC); i++)
		sem_wait (&semaforo_app);

	sleep(1);

	actualizar_estado_bloqueados();
	actualizar_estado_ejecutados();

}


void planificar_largo_plazo()
{
	while(!list_is_empty(cola_NEW)&& !list_is_empty(lista_repartidores_libres))
	{
		t_pedido* pedido = list_get(cola_NEW, 0);
		t_repartidor* repartidor_cercano = sacar_repartidor_mas_cercano(lista_repartidores_libres, pedido->posicion_de_restaurante);

		pedido->repartidor = repartidor_cercano;
		cambiar_estado_a(pedido, 1);

	}


}


void cambiar_estado_a(t_pedido* pedido, ESTADO_PCB estado_a_pasar)
{
	sacar_de_cola_actual(pedido);
	pedido->estado_pcb = estado_a_pasar;
	t_list* cola_nueva = dictionary_int_get(diccionario_colas, pedido->estado_pcb);
	meter_en_cola(pedido, cola_nueva);
	logear_cambio_cola(pedido, cola_nueva);
}




int sacar_de_cola_actual(t_pedido* pedido)
{

		t_list* cola = dictionary_int_get(diccionario_colas, pedido->estado_pcb);


		for(int i=0;i<list_size(cola) ;i++)
		{
			if(pedido->id_pedido == ((t_pedido*) list_get(cola, i))->id_pedido)
			{
				list_remove(cola, i);
				return 1;
			}
		}
		return -1;
}

void meter_en_cola(t_pedido* pedido, t_list* cola_nueva)
{
	if (cola_nueva == cola_READY)
		meter_en_cola_READY(pedido);
	 else
		list_add(cola_nueva, pedido);
}


void meter_en_cola_READY(t_pedido* pedido)
{
	if (strcmp(config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION"), "FIFO") == 0)
		meter_con_FIFO(pedido);

}


void meter_con_FIFO(t_pedido* pedido)  {list_add(cola_READY, pedido); }


void logear_cambio_cola(t_pedido* pedido, t_list* cola_nueva)
{
	if (cola_nueva == cola_READY)
		log_info(logger_app, "el pedido %d paso a la cola READY", pedido->id_pedido);
	if (cola_nueva == cola_EXEC)
			log_info(logger_app, "el pedido %d paso a la cola EXEC", pedido->id_pedido);
	if (cola_nueva == cola_BLOCKED)
			log_info(logger_app, "el pedido %d paso a la cola BLOCKED", pedido->id_pedido);
	if (cola_nueva == cola_EXIT)
			log_info(logger_app, "el pedido %d paso a la cola EXIT", pedido->id_pedido);

}


//-----------DICCIONARIO DE COLAS-----------//
void inicializar_diccionario_colas()
{
	diccionario_colas = dictionary_int_create();
	dictionary_int_put(diccionario_colas, NEW, cola_NEW);
	dictionary_int_put(diccionario_colas, READY, cola_READY);
	dictionary_int_put(diccionario_colas, BLOCKED, cola_BLOCKED);
	dictionary_int_put(diccionario_colas, EXEC, cola_EXEC);
	dictionary_int_put(diccionario_colas, EXIT, cola_EXIT);
}

