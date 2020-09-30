#include "planificacion.h"
#include "app.h"

t_list* cola_NEW;
t_list* cola_READY;
t_list* cola_BLOCKED;
t_list* cola_EXEC;
t_list* cola_EXIT;

t_dictionary_int* diccionario_colas;

//static int estimacion_proxima_rafaga(t_pedido* pedido);

void planificar_corto_plazo()
{
	if (strcmp(config_get_string_value(config_app, "ALGORITMO_DE_PLANIFICACION"), "HRRN") == 0)
		list_sort(cola_READY, (void*) &highest_ratio_response);

	while (grado_multiprocesamiento > list_size(cola_EXEC)&&(!list_is_empty(cola_READY)))
		cambiar_estado_a(list_remove(cola_READY, 0), EXEC);  //meter en cola EXEC (todavia no ejecuta)

}


static void actualizar_estado_listos()
{
	for (int i=0; i < list_size(cola_READY); i++)
	{
		t_pedido* pedido = list_get(cola_READY, i);
		pedido->ciclos_en_ready = pedido->ciclos_en_ready + 1;
	}

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
			pthread_cancel(pedido->hilo);
			pedido->repartidor = NULL;

		}
				else	{
			if (misma_posicion(pedido->repartidor->posicion, pedido->posicion_de_restaurante)) 	{
				if (pedido->esta_listo)
					pedido->instruccion_a_realizar = IR_A_CLIENTE;
				else {
					pedido->estimacion = (valor_alpha * pedido->ciclos_ejecutandose + (1 - valor_alpha) * pedido->estimacion);
					pedido->ciclos_ejecutandose = 0;
					cambiar_estado_a(pedido, BLOCKED);
					pedido->instruccion_a_realizar = ESPERAR_EN_RESTAURANTE;
				}
				}
			if (pedido->ciclos_ejecutandose == pedido->repartidor->frecuencia_descanso)	{
				pedido->estimacion = (valor_alpha * pedido->ciclos_ejecutandose + (1 - valor_alpha) * pedido->estimacion);
				pedido->ciclos_ejecutandose = 0;
				cambiar_estado_a(pedido, BLOCKED);
			}
		}
	}
}

void ejecutar_ciclo()
{
	void ejecutar_pedido(t_pedido* pedido) { sem_post (&(pedido->mutex)); }
	list_iterate(cola_EXEC, (void*) &ejecutar_pedido); //Ejecuta cada pedido en las cola EXEC

	void esperar_pedido(t_pedido* pedido) { sem_wait (&semaforo_app); }
	list_iterate(cola_EXEC, (void*) &esperar_pedido); //Espera que los pedidos en EXEC terminen de ejecutar

	//sleep(1);
	//sleep(config_get_int_value(config, "CICLO"));
	actualizar_estado_listos();
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
	if (strcmp(config_get_string_value(config_app, "ALGORITMO_DE_PLANIFICACION"), "FIFO") == 0)
		meter_con_FIFO(pedido);

	if (strcmp(config_get_string_value(config_app, "ALGORITMO_DE_PLANIFICACION"), "SJF-SD") == 0)
		meter_con_SJF_SD(pedido);

	if (strcmp(config_get_string_value(config_app, "ALGORITMO_DE_PLANIFICACION"), "HRRN") == 0)
		meter_con_FIFO(pedido);
}

void meter_con_FIFO(t_pedido* pedido)  {list_add(cola_READY, pedido); }

void meter_con_SJF_SD(t_pedido* pedido)
{
	int i = 0;
	t_pedido* pivot = list_get(cola_READY, i);
	while(pivot != NULL && (pivot->estimacion <=pedido->estimacion))
	{
		i++;
		pivot = list_get(cola_READY, i);
	}
	list_add_in_index(cola_READY, i, pedido);

}

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

/*
static int estimacion_proxima_rafaga(t_pedido* pedido)
{
	pedido->estimacion = (valor_alpha * pedido->ciclos_anterior_rafaga + (1 - valor_alpha) * pedido->estimacion);
	return pedido->estimacion;
}
*/

bool highest_ratio_response(t_pedido* pedido1, t_pedido* pedido2)
{
	if ((pedido1->ciclos_en_ready / pedido1->estimacion + 1) > (pedido2->ciclos_en_ready / pedido2->estimacion + 1))
		return true;
		else return false;

}

float convertir_string_en_float (char* token)
{

	for (int i=0; i < strlen(token); i++)
	{
		if (token[i] == ','){
			token[i]='.';
			break;
		}
	}
	return strtof(token, NULL);
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

