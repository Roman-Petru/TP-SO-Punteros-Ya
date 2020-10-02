#include "planificacionR.h"

static void actualizar_estado_hornos();
static void actualizar_estado_bloqueados();
static void actualizar_estado_ejecutados();

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

	int semaforos_de_reposar = 0;
	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		void ejecutar_plato(t_platos_PCB* plato) { sem_post (&(plato->mutex)); }
		list_iterate(list_get(cola_de_cola_Resto_EXEC, i), (void*) &ejecutar_plato); //Ejecuta cada pedido en las cola EXEC
	}

		void ejecutar_horno(t_platos_PCB* plato) { sem_post (&(plato->mutex)); }
		list_iterate(cola_Hornos_EXEC, (void*) &ejecutar_horno);

		void ejecutar_reposados(t_platos_PCB* plato) {

			if (!list_is_empty(plato->pasos_receta_faltantes)  && strcmp (((t_paso*) list_get(plato->pasos_receta_faltantes, 0))->operacion, "REPOSAR") == 0)
			{
				sem_post (&(plato->mutex));
				semaforos_de_reposar++;
			}
			}

		list_iterate(cola_Resto_BLOCKED, (void*) &ejecutar_reposados);


	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		void esperar_plato(t_platos_PCB* plato) { sem_wait (&semaforo_resto); }
		list_iterate(list_get(cola_de_cola_Resto_EXEC, i), (void*) &esperar_plato);  //Espera ejecuciones
	}

		void esperar_plato(t_platos_PCB* plato) { sem_wait (&semaforo_resto); }
		list_iterate(cola_Hornos_EXEC, (void*) &esperar_plato);


		for (int i=0; i<semaforos_de_reposar; i++)
		{
			sem_wait (&semaforo_resto);
		}

		sleep(1);

		actualizar_estado_hornos();
		actualizar_estado_bloqueados();
		actualizar_estado_ejecutados();

}


static void	actualizar_estado_hornos()
{
	for (int i=0; i < list_size(cola_Hornos_EXEC); i++)
		{
			t_platos_PCB* plato = list_get(cola_Hornos_EXEC, i);
			t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
			if (list_is_empty(plato->pasos_receta_faltantes) || strcmp(paso->operacion, "HORNEAR") != 0)
				list_remove(cola_Hornos_EXEC, i);
		}
}

static void actualizar_estado_bloqueados()
{

	for (int i=0; i < list_size(cola_Resto_BLOCKED); i++)
		{
			t_platos_PCB* plato = list_get(cola_Resto_BLOCKED, i);
			if (list_is_empty(plato->pasos_receta_faltantes))
			{
				terminar_plato(plato);
			} else if (plato->hubo_cambio_operacion){
				plato->hubo_cambio_operacion = false;
				t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
				if (strcmp(paso->operacion, "HORNEAR") == 0)
					list_add(cola_Hornos_READY, plato);
				if (strcmp(paso->operacion, "REPOSAR") != 0 && strcmp(paso->operacion, "HORNEAR") != 0)
					cambiar_estado_a(plato, READY);
			}
	}
}


	//VERSION KILOMBO DE actualizar_estado_bloqueados, SIN EJECUTAR BLOQUEADOS
/*	for (int i=0; i < list_size(cola_Resto_BLOCKED); i++)
	{
		t_platos_PCB* plato = list_get(cola_Resto_BLOCKED, i);
		if (list_is_empty(plato->pasos_receta_faltantes))
		{
			//sacar de lista de hornos si estaban
			terminar_plato(plato);
		} else {
		t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
		if (strcmp(paso->operacion, "REPOSAR") == 0)
		{
			if (avanzar_paso_receta(plato))   //ESPERO QUE NO PUEDAN PONER 2 VECES SEGUIDAS REPOSAR
				{
				if (strcmp(paso->operacion, "HORNEAR") == 0)
				{
					list_add(cola_Hornos_READY, plato);
				}
				else cambiar_estado_a(plato, READY);
				}
		} else if (strcmp(paso->operacion, "HORNEAR") != 0)
				cambiar_estado_a(plato, READY);
		//sacar de lista de hornos si estaban
		}
	}*/


static void actualizar_estado_ejecutados()
{
	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		for(int j=0; j < list_size(list_get(cola_de_cola_Resto_EXEC, i)); j++)
		{
			t_platos_PCB* plato = list_get(list_get(cola_de_cola_Resto_EXEC, i), j);
			if (list_is_empty(plato->pasos_receta_faltantes))
			{
				terminar_plato(plato);
			} else
			{
				t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
				if (strcmp(paso->operacion, "HORNEAR") == 0)
				{
					cambiar_estado_a(plato, BLOCKED);
					list_add(cola_Hornos_READY, plato);
				} else if (strcmp(paso->operacion, "REPOSAR") == 0)
					{
					cambiar_estado_a(plato, BLOCKED);
//----------------------------------------------------------------SI ES ALGORITMO RR--------------------------//
					}	else if (strcmp(config_get_string_value(config_resto, "ALGORITMO_PLANIFICACION"), "RR") == 0  && plato->ciclos_ejecutandose == quantum)
						cambiar_estado_a(plato, READY);
			}
		}
	}
}

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


			//if(plato->id_PCB == ((t_platos_PCB*) list_get(list_get(cola, j), i))->id_PCB)

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


void inicializar_diccionario_colas()
{
	diccionario_colas = dictionary_int_create();
	dictionary_int_put(diccionario_colas, NEW, cola_Resto_NEW);
	dictionary_int_put(diccionario_colas, READY, cola_de_cola_Resto_READY);
	dictionary_int_put(diccionario_colas, BLOCKED, cola_Resto_BLOCKED);
	dictionary_int_put(diccionario_colas, EXEC, cola_de_cola_Resto_EXEC);
	dictionary_int_put(diccionario_colas, EXIT, cola_Resto_EXIT);
}
