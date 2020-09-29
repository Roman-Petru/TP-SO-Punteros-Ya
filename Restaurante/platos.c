#include "platos.h"



t_platos_PCB* crear_plato(char* plato, int id_pedido)
{
	t_platos_PCB* nuevo_plato = malloc(sizeof(t_platos_PCB));

	nuevo_plato->id_pedido = id_pedido;
	nuevo_plato->nombre_plato = plato;
	nuevo_plato->paso_actual = 0;
	nuevo_plato->estado_pcb = READY;
	nuevo_plato->ciclos_ejecutandose = 0;

	//sem_init(&(nuevo_plato->mutex), 0, 0);
	//pthread_create(&(nuevo_plato->hilo), NULL, (void*) &ciclo_plato, nuevo_plato);
	//pthread_detach(&(nuevo_plato->hilo));


	return nuevo_plato;

}


