#ifndef PLANI_APP
#define PLANI_APP

#include <commons/collections/list.h>
#include "../Red/dictionary_int.h"
#include <pthread.h>
#include <string.h>
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
void cambiar_estado_a(t_pedido* pedido, ESTADO_PCB estado_a_pasar);
int sacar_de_cola_actual(t_pedido* pedido);
void meter_en_cola(t_pedido* pedido, t_list* cola_nueva);
void meter_en_cola_READY(t_pedido* pedido);
void meter_con_FIFO(t_pedido* pedido);


#endif //PLANI_APP

