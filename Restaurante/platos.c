#include "platos.h"



t_platos_PCB* crear_plato(char* plato, int id_pedido)
{
	t_platos_PCB* nuevo_plato = malloc(sizeof(t_platos_PCB));

	nuevo_plato->id_pedido = id_pedido;
	nuevo_plato->nombre_plato = plato;
	nuevo_plato->ciclos_restantes_paso_actual = 0;
	nuevo_plato->estado_pcb = READY;
	nuevo_plato->ciclos_ejecutandose = 0;

	sem_init(&(nuevo_plato->mutex), 0, 0);
	pthread_create(&(nuevo_plato->hilo), NULL, (void*) &ciclo_plato, nuevo_plato);
	//pthread_detach(&(nuevo_plato->hilo));


	return nuevo_plato;

}




void ciclo_plato(t_platos_PCB* plato)
{
	while(1)
	{
	sem_wait(&(plato->mutex));
	//((t_accion) dictionary_int_get(diccionario_acciones, pedido->instruccion_a_realizar))(pedido); avanzar paso?
	avanzar_paso_receta(plato);
	plato->ciclos_ejecutandose++;
	//sem_post(&(semaforo_resto));    //SI INCLUYO SE ROMPE POR INCLUDE CIRCULAR
	}
}


bool avanzar_paso_receta (t_platos_PCB* plato)
{
	plato->ciclos_restantes_paso_actual--;

	if (plato->ciclos_restantes_paso_actual == 0)
	{
		list_remove(plato->pasos_receta_faltantes, 0);
		if (!list_is_empty(plato->pasos_receta_faltantes))
		{
			t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
			plato->ciclos_restantes_paso_actual = paso->ciclos;
			return true;
		}
	}
	return false;
}

