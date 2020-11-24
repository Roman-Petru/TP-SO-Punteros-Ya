#include "planificacionR.h"

int quantum;
int id_PCB;

t_list* cola_Resto_NEW;
t_list* cola_de_cola_Resto_READY;
t_list* cola_Resto_BLOCKED;
t_list* cola_de_cola_Resto_EXEC;
t_list* cola_Resto_EXIT;

t_list* cola_Hornos_READY;
t_list* cola_Hornos_EXEC;

sem_t semaforo_resto;
sem_t mutex_cliente;

static void actualizar_estado_bloqueados();
static void actualizar_estado_ejecutados();
static bool termino_horneado(t_platos_PCB* plato);


void planificar_corto_plazo()
{
	//----------------------------PLANIFICAR PLATOS ------------------------//

	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		t_afinidad* afinidad = list_get(lista_afinidades, i);
		while (afinidad->cantidad_cocineros > list_size(list_get(cola_de_cola_Resto_EXEC, i))&&(!list_is_empty(list_get(cola_de_cola_Resto_READY, i))))
		{
		logear_inicio_operacion(list_get(list_get(cola_de_cola_Resto_READY, i), 0));
			//log_info(logger_resto, "El plato con PCB %d ahora proseguira a %s", ((t_platos_PCB*) list_get(list_get(cola_de_cola_Resto_READY, i), 0))->id_PCB);
			cambiar_estado_a(list_get(list_get(cola_de_cola_Resto_READY, i), 0), EXEC);
		}
	}

	//----------------------------PLANIFICAR HORNOS ------------------------//

	while (cantidad_hornos > list_size(cola_Hornos_EXEC)&&(!list_is_empty(cola_Hornos_READY)))
	{
		logear_inicio_operacion(list_get(cola_Hornos_READY, 0));
		list_add(cola_Hornos_EXEC, list_remove(cola_Hornos_READY, 0));
	}
}

void ejecutar_ciclo()
{
//--------------------------------- SE ABREN SEMAFOROS -----------------------------------//
	int semaforos_de_reposar = 0;
	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		void ejecutar_plato(t_platos_PCB* plato) { sem_post (&(plato->mutex)); }
		list_iterate(list_get(cola_de_cola_Resto_EXEC, i), (void*) &ejecutar_plato);
	}

		void ejecutar_horno(t_platos_PCB* plato) { sem_post (&(plato->mutex)); }
		list_iterate(cola_Hornos_EXEC, (void*) &ejecutar_horno);

		void ejecutar_reposados(t_platos_PCB* plato) {

			if (!list_is_empty(plato->pasos_receta_faltantes)  && strcmp (((t_paso*) list_get(plato->pasos_receta_faltantes, 0))->operacion, "Reposar") == 0)
			{
				sem_post (&(plato->mutex));
				semaforos_de_reposar++;
			}
			}

		list_iterate(cola_Resto_BLOCKED, (void*) &ejecutar_reposados);

//--------------------------------- SE ESPERAN LOS SEMAFOROS --------------------------------//

	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		void esperar_plato(t_platos_PCB* plato) { sem_wait (&semaforo_resto); }
		list_iterate(list_get(cola_de_cola_Resto_EXEC, i), (void*) &esperar_plato);
	}

		void esperar_plato(t_platos_PCB* plato) { sem_wait (&semaforo_resto); }
		list_iterate(cola_Hornos_EXEC, (void*) &esperar_plato);


		for (int i=0; i<semaforos_de_reposar; i++)
		{
			sem_wait (&semaforo_resto);
		}

		sleep(config_get_int_value(config_resto, "RETARDO_CICLO_CPU"));


		actualizar_estado_bloqueados();
		actualizar_estado_ejecutados();
}


static void actualizar_estado_bloqueados()
{

	for (int i=0; i < list_size(cola_Resto_BLOCKED); i++)
		{
			t_platos_PCB* plato = list_get(cola_Resto_BLOCKED, i);

			if (list_is_empty(plato->pasos_receta_faltantes) || termino_horneado(plato))
				{
				cambiar_paso_de_ser_necesario(plato);
				//((t_insertador_terminar_hornos) dictionary_int_get(diccionario_interrupciones, TERMINAR_HORNEADO))(plato);
				agregar_interrupcion(TERMINAR_HORNEADO, plato);
				} else {
			cambiar_paso_de_ser_necesario(plato);
			if (list_is_empty(plato->pasos_receta_faltantes))
			{
				terminar_plato(plato);
				i--;
			} else if (plato->hubo_cambio_operacion){
				plato->hubo_cambio_operacion = false;
				t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
				if (strcmp(paso->operacion, "Hornear") == 0)
					list_add(cola_Hornos_READY, plato);
				if (strcmp(paso->operacion, "Reposar") == 0)
					logear_inicio_operacion(plato);
				if (strcmp(paso->operacion, "Reposar") != 0 && strcmp(paso->operacion, "Hornear") != 0)
					cambiar_estado_a(plato, READY);
					i--;
			}
			}
	}
}

static void actualizar_estado_ejecutados()
{
	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		for(int j=0; j < list_size(list_get(cola_de_cola_Resto_EXEC, i)); j++)
		{
			t_platos_PCB* plato = list_get(list_get(cola_de_cola_Resto_EXEC, i), j);
			cambiar_paso_de_ser_necesario(plato);

			if (plato->hubo_cambio_operacion)
			{
			if (list_is_empty(plato->pasos_receta_faltantes))
			{
				terminar_plato(plato);
				j--;
			} else	{
				t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
				plato->hubo_cambio_operacion = false;
				if (strcmp(paso->operacion, "Hornear") == 0)
				{
					cambiar_estado_a(plato, BLOCKED);
					plato->ciclos_ejecutandose = 0;
					j--;
					list_add(cola_Hornos_READY, plato);
				} else if (strcmp(paso->operacion, "Reposar") == 0)
					{
					cambiar_estado_a(plato, BLOCKED);
					plato->ciclos_ejecutandose = 0;
					j--;
					logear_inicio_operacion(plato);

//----------------------------------------------------SI CAMBIA DE OPERACION PERO SIGUE EN EXEC Y JUSTO TAMBIEN TERMINA QUANTUM--------------------------//
							} else if (strcmp(paso->operacion, "Reposar") != 0 && strcmp(paso->operacion, "Hornear") != 0)
								{
								if (strcmp(config_get_string_value(config_resto, "ALGORITMO_PLANIFICACION"), "RR") == 0  && plato->ciclos_ejecutandose == quantum)
									{
									cambiar_estado_a(plato, READY);
									plato->ciclos_ejecutandose = 0;
									j--;
									} else {
//----------------------------------------------------SI CAMBIA DE OPERACION PERO SIGUE EN EXEC Y NO TERMINA QUANTUM--------------------------//
								logear_inicio_operacion(plato);
								}}}
//------------------------------------------------------SI TERMINA QUANTUM SIN CAMBIO DE OPERACION--------------------------------------//
		}	else if (strcmp(config_get_string_value(config_resto, "ALGORITMO_PLANIFICACION"), "RR") == 0  && plato->ciclos_ejecutandose == quantum)
			{
			cambiar_estado_a(plato, READY);
			log_info(logger_resto, "Se pauso la operacion de %s del plato de %s con PCB %d debido a fin de quantum", (((t_paso*) list_get(plato->pasos_receta_faltantes, 0))->operacion), plato->nombre_plato, plato->id_PCB);
			plato->ciclos_ejecutandose = 0;
			j--;
			}
}}}

void cambiar_estado_a(t_platos_PCB* plato, ESTADO_PCB estado_a_pasar)
{
	sacar_de_cola_actual(plato);
	plato->estado_pcb = estado_a_pasar;
	t_list* cola_nueva = dictionary_int_get(diccionario_colas, plato->estado_pcb);
	meter_en_cola(plato, cola_nueva);
	//log_info(logger_resto, "Cambio de cola del plato %d a cola %d", plato->id_PCB, estado_a_pasar);
}

int sacar_de_cola_actual(t_platos_PCB* plato)
{

		t_list* cola = dictionary_int_get(diccionario_colas, plato->estado_pcb);

		if (cola == cola_de_cola_Resto_EXEC || cola == cola_de_cola_Resto_READY)
		{
			int j = numero_de_cola(plato->nombre_plato);

			for(int i=0;i<list_size(list_get(cola, j)) ;i++)
			{
				t_platos_PCB* pivot = list_get(list_get(cola, j), i);

				if(plato->id_PCB == pivot->id_PCB)
				{
				list_remove(list_get(cola, j), i);
				return 1;
				}
			}

		}	else	{
			for(int i=0;i<list_size(cola) ;i++)
					{
			if(plato->id_pedido == ((t_platos_PCB*) list_get(cola, i))->id_pedido)
			{
				list_remove(cola, i);
				return 1;
			}
		}
		}
		return -1;
}

void meter_en_cola_EXEC(t_platos_PCB* plato)
{
	int i = numero_de_cola(plato->nombre_plato);
	list_add (list_get(cola_de_cola_Resto_EXEC, i), plato);
}

void meter_en_cola(t_platos_PCB* plato, t_list* cola_nueva)
{
	if (cola_nueva == cola_de_cola_Resto_READY)
		meter_en_cola_READY(plato);
	 else if (cola_nueva == cola_de_cola_Resto_EXEC)
		 meter_en_cola_EXEC(plato);
	 else
		list_add(cola_nueva, plato);
}

void meter_en_cola_READY(t_platos_PCB* plato)
{
	int i = numero_de_cola(plato->nombre_plato);
	list_add (list_get(cola_de_cola_Resto_READY, i), plato);
}

int numero_de_cola (char* nombre)
{
	int i;
	for (i=0; i<((list_size(lista_afinidades))-1);i++)
	{
		t_afinidad* afinidad = list_get(lista_afinidades, i);
		if (strcmp(nombre, afinidad->plato) == 0)
			return i;
	}
	return i;
}


static bool termino_horneado(t_platos_PCB* plato){return (strcmp(((t_paso*) list_get(plato->pasos_receta_faltantes, 0))->operacion, "Hornear") == 0 && plato->ciclos_restantes_paso_actual == 0);}


void sacar_del_horno(t_platos_PCB* plato)
{
	for(int i=0; i < list_size(cola_Hornos_EXEC); i++)
	{
			t_platos_PCB* pivot = list_get(cola_Hornos_EXEC, i);
			if (plato->id_PCB == pivot->id_PCB)
			{
				list_remove(cola_Hornos_EXEC, i);
				return;
			}
	}
}

void inicializar_planificador()
{

	sem_init (&(semaforo_resto), 0, 0);
	sem_init (&(mutex_cliente), 0, 1);

		cola_Resto_NEW = list_create();
		cola_Resto_BLOCKED = list_create();

		cola_de_cola_Resto_READY = list_create();
		cola_de_cola_Resto_EXEC = list_create();
		for (int i=0; i<list_size(lista_afinidades);i++){
			t_list* cola_Resto_READY;
			cola_Resto_READY = list_create();
			list_add(cola_de_cola_Resto_READY, cola_Resto_READY);

			t_list* cola_Resto_EXIT;
			cola_Resto_EXIT = list_create();
			list_add(cola_de_cola_Resto_EXEC, cola_Resto_EXIT);

		}

		cola_Resto_EXIT = list_create();
		cola_Hornos_READY = list_create();
		cola_Hornos_EXEC = list_create();

		inicializar_diccionario_colas();

		quantum = config_get_int_value(config_resto, "QUANTUM");
		id_PCB = 0;
}

void inicializar_diccionario_colas()
{
	diccionario_colas = dictionary_int_create();
	dictionary_int_put(diccionario_colas, NEW, cola_Resto_NEW);
	dictionary_int_put(diccionario_colas, READY, cola_de_cola_Resto_READY);
	dictionary_int_put(diccionario_colas, BLOCKED, cola_Resto_BLOCKED);
	dictionary_int_put(diccionario_colas, EXEC, cola_de_cola_Resto_EXEC);
	dictionary_int_put(diccionario_colas, EXIT, cola_Resto_EXIT);
}



