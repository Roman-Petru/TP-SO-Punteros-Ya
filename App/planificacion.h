#ifndef PLANI_APP
#define PLANI_APP

#include <commons/collections/list.h>
#include <pthread.h>
#include "repartidores.h"

extern t_list* lista_repartidores_libres;

typedef enum{
	NEW = 0,
	READY = 1,
	EXEC = 2,
	BLOCKED = 3,
	EXIT = 4
}ESTADO_PCB;

typedef enum{
	IR_A_RESTAURANTE = 0,
	ESPERAR_EN_RESTAURANTE = 1,
	IR_A_CLIENTE = 2
}INSTRUCCION_A_REALIZAR;


typedef struct{
	int id_pedido;
	t_repartidor* repartidor;
	t_posicion* posicion_de_restaurante;

	int ciclos_ejecutandose;
	int ciclos_descansando;

	ESTADO_PCB estado_pcb;
	INSTRUCCION_A_REALIZAR instruccion_a_realizar;

	pthread_t hilo;
	pthread_mutex_t mutex;

}t_pedido;


void planificador_largo_plazo();

#endif //PLANI_APP

