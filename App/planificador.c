#include "planificador.h"
#include "app.h"
#include "../Utils/posicion.h"

t_list* cola_NEW;
t_list* cola_READY;
t_list* cola_BLOCKED;
t_list* cola_EXEC;
t_list* cola_EXIT;

t_dictionary_int* diccionario_colas;
t_dictionary_int* diccionario_estado_string;
t_dictionary_int* diccionario_razones;
t_dictionary* diccionario_algoritmos;

static void meter_con_FIFO(t_pedido* pedido);
static void meter_con_SJF_SD(t_pedido* pedido);
static bool highest_response_ratio(t_pedido* pedido1, t_pedido* pedido2);

//========== PLANIFICADOR ==========//
void inicializar_planificador()
{
	//===COLAS===//
	cola_NEW = list_create();
	cola_READY = list_create();
	cola_BLOCKED = list_create();
	cola_EXEC = list_create();
	cola_EXIT = list_create();

	//===DICCIONARIO DE COLAS===//
	diccionario_colas = dictionary_int_create();
	dictionary_int_put(diccionario_colas, NEW, cola_NEW);
	dictionary_int_put(diccionario_colas, READY, cola_READY);
	dictionary_int_put(diccionario_colas, BLOCKED, cola_BLOCKED);
	dictionary_int_put(diccionario_colas, EXEC, cola_EXEC);
	dictionary_int_put(diccionario_colas, EXIT, cola_EXIT);

	//===DICCIONARIO DE ESTADO_STRING===//
	diccionario_estado_string = dictionary_int_create();
	dictionary_int_put(diccionario_estado_string, NEW, "NEW");
	dictionary_int_put(diccionario_estado_string, EXEC, "EXEC");
	dictionary_int_put(diccionario_estado_string, READY, "READY");
	dictionary_int_put(diccionario_estado_string, BLOCKED, "BLOCKED");
	dictionary_int_put(diccionario_estado_string, EXIT, "EXIT");

	//===DICCIONARIO DE RAZONES===//
	diccionario_razones = dictionary_int_create();
	dictionary_int_put(diccionario_razones, ASIGNO_PEDIDO, "fin de quantum");
	dictionary_int_put(diccionario_razones, ENTREGO_PEDIDO, "entreda/salida");
	dictionary_int_put(diccionario_razones, CANSADO, "fin de ejecucion");
	dictionary_int_put(diccionario_razones, DESCANSO, "asignarsele una captura pokemon");
	dictionary_int_put(diccionario_razones, FIN_QUANTUM, "asignarsele un intercambio");

	//===DICCIONARIO DE ALGORITMOS===//
	diccionario_algoritmos = dictionary_create();
	dictionary_put(diccionario_algoritmos, "FIFO", &meter_con_FIFO);
	dictionary_put(diccionario_algoritmos, "SJF-SD", &meter_con_SJF_SD);
	dictionary_put(diccionario_algoritmos, "HRRN", &meter_con_FIFO);
}

void planificar_corto_plazo()
{
	if (strcmp(config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION"), "HRRN") == 0)
		list_sort(cola_READY, (void*) &highest_response_ratio);

	while (config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO") > list_size(cola_EXEC)&&(!list_is_empty(cola_READY)))
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
	void actualizar_estado(t_pedido* pedido)
	{
		if (pedido->instruccion_a_realizar == ESPERAR_EN_RESTAURANTE)
		{
			if (pedido->esta_listo)
			{
				cambiar_estado_a(pedido, READY);
				pedido->instruccion_a_realizar = IR_A_CLIENTE;
			}
		}
		else
		{
			//TODO: Revisar Posible Bug
			repartidor_descansar(pedido->repartidor);
			if (!pedido->repartidor->esta_cansado)
				cambiar_estado_a(pedido, READY);
		}
	}

	list_iterate(cola_BLOCKED, (void*) &actualizar_estado);
	/*
	for (int i=0; i < list_size(cola_BLOCKED); i++)
	{
		t_pedido* pedido = list_get(cola_BLOCKED, i);
		if (pedido->instruccion_a_realizar == ESPERAR_EN_RESTAURANTE)
		{
			if (!pedido->esta_listo)
				continue;
			cambiar_estado_a(pedido, READY);
			pedido->instruccion_a_realizar = IR_A_CLIENTE;
		}
		else
		{
			//TODO: Revisar Posible Bug
			repartidor_descansar(pedido->repartidor);
			if (!pedido->repartidor->esta_cansado)
				cambiar_estado_a(pedido, READY);
		}
	}*/
}

static void actualizar_estado_ejecutados()
{
	//BUGGG descubierto, el list_size(COLA_EXEC) (y la cola en si) pueden cambiar mientras se itera, esta iteracion puede no funcionar bien,
	//POSIBLE SOLUCION: agregar un i--; CUANDO SE CAMBIE DE ESTADO UN PCB

	for (int i=0; i < list_size(cola_EXEC); i++)
	{
		t_pedido* pedido = list_get(cola_EXEC, i);
		if (pedido->instruccion_a_realizar == IR_A_CLIENTE && posicion_es_igual(pedido->repartidor->posicion, pedido->posicion_cliente))
		{
			log_info(logger, "el pedido %d entrego el pedido al cliente", pedido->id_pedido);
			cambiar_estado_a(pedido, EXIT);
			list_add(repartidores_libres, pedido->repartidor);
			pthread_cancel(pedido->hilo);
			pedido->repartidor = NULL;

		}
		else
		{
			if (posicion_es_igual(pedido->repartidor->posicion, pedido->posicion_de_restaurante))
			{
				if (pedido->esta_listo)
					pedido->instruccion_a_realizar = IR_A_CLIENTE;
				else
				{
					cambiar_estado_a(pedido, BLOCKED);
					pedido->instruccion_a_realizar = ESPERAR_EN_RESTAURANTE;
				}
			}
			if (pedido->repartidor->esta_cansado)
				cambiar_estado_a(pedido, BLOCKED);

		}
	}
}

void ejecutar_ciclo()
{
	void ejecutar_pedido(t_pedido* pedido) { sem_post (&(pedido->mutex)); }
	list_iterate(cola_EXEC, (void*) &ejecutar_pedido); //Ejecuta cada pedido en las cola EXEC

	void esperar_pedido(t_pedido* pedido) { sem_wait (&sem_ciclo); }
	list_iterate(cola_EXEC, (void*) &esperar_pedido); //Espera que los pedidos en EXEC terminen de ejecutar

	//sleep(1);
	//sleep(config_get_int_value(config, "CICLO"));
	actualizar_estado_listos();
	actualizar_estado_bloqueados();
	actualizar_estado_ejecutados();
}

void planificar_largo_plazo()
{
	while(!list_is_empty(cola_NEW)&& !list_is_empty(repartidores_libres))
	{
		t_pedido* pedido = list_get(cola_NEW, 0);
		t_repartidor* repartidor_cercano = repartidor_obtener_mas_cercano(pedido->posicion_de_restaurante);

		pedido->repartidor = repartidor_cercano;
		cambiar_estado_a(pedido, 1);
	}
}

//========== COLAS ==========//
static void sacar_de_cola_actual(t_pedido* pedido)
{
	t_list* cola = dictionary_int_get(diccionario_colas, pedido->estado_pcb);

	for(int i=0;i<list_size(cola) ;i++)
	{
		if(pedido_es_mismo(pedido, list_get(cola, i)))
			list_remove(cola, i);
	}
}

static void meter_en_cola_READY(t_pedido* pedido)
{
	((t_insertador) dictionary_get(diccionario_algoritmos, config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION")))(pedido);
}

void meter_en_cola(t_pedido* pedido, ESTADO_PCB estado)
{
	if (estado == READY)
		meter_en_cola_READY(pedido);
	else
		list_add(dictionary_int_get(diccionario_colas, estado), pedido);
	pedido->estado_pcb = estado;
	log_info(logger, "El pedido %d paso a la cola %s", pedido->id_pedido, dictionary_int_get(diccionario_estado_string, estado));
}

void cambiar_estado_a(t_pedido* pedido, ESTADO_PCB estado)
{
	sacar_de_cola_actual(pedido);
	meter_en_cola(pedido, estado);
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

//========== FIFO ==========//
static void meter_con_FIFO(t_pedido* pedido)  { list_add(cola_READY, pedido); }

//========== SJF_SD ==========//
static void meter_con_SJF_SD(t_pedido* pedido)
{
	int i = 0;
	for(t_pedido* pivot = list_get(cola_READY, i); pivot != NULL && (pivot->estimacion <=pedido->estimacion); pivot = list_get(cola_READY, i))
		i++;
	list_add_in_index(cola_READY, i, pedido);
	/*
	int i = 0;
	t_pedido* pivot = list_get(cola_READY, i);
	while(pivot != NULL && (pivot->estimacion <=pedido->estimacion))
	{
		i++;
		pivot = list_get(cola_READY, i);
	}
	list_add_in_index(cola_READY, i, pedido);
	*/
}

//========== HRRN ==========//
bool highest_response_ratio(t_pedido* pedido1, t_pedido* pedido2)
{
	return (pedido1->ciclos_en_ready / pedido1->estimacion + 1) > (pedido2->ciclos_en_ready / pedido2->estimacion + 1);
}

//========== PARA TEST ==========//
void recibir_pedidos_default(int cantidad)
{
	for(int i=1;i<cantidad+1;i++)
		meter_en_cola(crear_pedido_default(i), NEW);
}

