#include "planificacionR.h"

static void actualizar_estado_bloqueados();
static void actualizar_estado_ejecutados();

void planificar_corto_plazo()
{
	//----------------------------PLANIFICAR PLATOS ------------------------//

	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		t_afinidad* afinidad = list_get(lista_afinidades, i);
		while (afinidad->cantidad_cocineros > list_size(list_get(cola_de_cola_Resto_EXEC, i))&&(!list_is_empty(list_get(cola_de_cola_Resto_READY, i))))
				cambiar_estado_a(list_remove(list_get(cola_de_cola_Resto_READY, i), 0), EXEC);
	}

	//----------------------------PLANIFICAR HORNOS ------------------------//

	while (cantidad_hornos > list_size(cola_Hornos_EXEC)&&(!list_is_empty(cola_Hornos_READY)))
		list_add(cola_Hornos_EXEC, list_remove(cola_Hornos_READY, 0));

}

void ejecutar_ciclo()
{

	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		void ejecutar_plato(t_platos_PCB* plato) { sem_post (&(plato->mutex)); }
		list_iterate(list_get(cola_de_cola_Resto_EXEC, i), (void*) &ejecutar_plato); //Ejecuta cada pedido en las cola EXEC
	}

		void ejecutar_horno(t_platos_PCB* plato) { sem_post (&(plato->mutex)); }  //REPENSAR ESTA PARTE
		list_iterate(cola_Hornos_EXEC, (void*) &ejecutar_horno);


	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		void esperar_plato(t_platos_PCB* plato) { sem_wait (&semaforo_resto); }
		list_iterate(list_get(cola_de_cola_Resto_EXEC, i), (void*) &esperar_plato);  //Espera ejecuciones
	}

		void esperar_plato(t_platos_PCB* plato) { sem_wait (&semaforo_resto); }
		list_iterate(cola_Hornos_EXEC, (void*) &esperar_plato);


		//sleep(1);

		actualizar_estado_bloqueados();
		actualizar_estado_ejecutados();

}

static void actualizar_estado_bloqueados()
{
	for (int i=0; i < list_size(cola_Resto_BLOCKED); i++)
	{
		t_platos_PCB* plato = list_get(cola_Resto_BLOCKED, i);
		t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
		if (strcmp(paso->operacion, "REPOSAR") == 0)
		{
			if (avanzar_paso_receta(plato))   //ESPERO QUE NO PUEDAN PONER 2 VECES SEGUIDAS REPOSAR
				cambiar_estado_a(plato, READY);
		}
	}
}

static void actualizar_estado_ejecutados()
{
	for (int i=0; i<list_size(lista_afinidades); i++)
	{
		for(int j=0; j < list_size(list_get(cola_de_cola_Resto_EXEC, i)); j++)
		{
			t_platos_PCB* plato = list_get(cola_de_cola_Resto_EXEC, i);
			if (list_is_empty(plato->pasos_receta_faltantes))
			{
				cambiar_estado_a(plato, EXIT);
			} else
			{
				t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
				if (strcmp(paso->operacion, "HORNEAR") == 0)
				{
					cambiar_estado_a(plato, BLOCKED);
					list_add(cola_Hornos_READY, plato);
				} else if (strcmp(paso->operacion, "REPOSAR") == 0)
					cambiar_estado_a(plato, BLOCKED);
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
	//logear_cambio_cola(plato, cola_nueva);
}




int sacar_de_cola_actual(t_platos_PCB* pedido)
{

		t_list* cola = dictionary_int_get(diccionario_colas, pedido->estado_pcb);


		for(int i=0;i<list_size(cola) ;i++)
		{
			if(pedido->id_pedido == ((t_platos_PCB*) list_get(cola, i))->id_pedido)
			{
				list_remove(cola, i);
				return 1;
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
	//dictionary_int_put(diccionario_colas, HORNO, cola_Horneados);
}
