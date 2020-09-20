#ifndef PEDIDOS_APP
#define PEDIDOS_APP

#include "repartidores.h"
#include "stddef.h"
#include <pthread.h>
#include "../Utils/dictionary_int.h"
#include <semaphore.h>
#include <stdbool.h>

extern t_dictionary_int* diccionario_acciones;

typedef enum{
	NEW = 0,
	READY = 1,
	EXEC = 2,
	BLOCKED = 3,
	EXIT = 4
}ESTADO_PCB;

typedef enum{
	IR_A_RESTAURANTE = 0,
	IR_A_CLIENTE = 1,
	ESPERAR_EN_RESTAURANTE = 2
}INSTRUCCION_A_REALIZAR;


typedef struct{
	int id_pedido;
	t_repartidor* repartidor;
	t_posicion* posicion_de_restaurante;
	t_posicion* posicion_cliente;


	int ciclos_ejecutandose;
	int ciclos_descansados;
	bool esta_listo;


	ESTADO_PCB estado_pcb;
	INSTRUCCION_A_REALIZAR instruccion_a_realizar;

	pthread_t hilo;
	sem_t mutex;

}t_pedido;

t_pedido* crear_pedido(int id, t_posicion* posicion_de_restaurante, t_posicion* posicion_cliente, bool resto_default);
void ciclo_pedido(t_pedido* pedido);
typedef void (*t_accion)(t_pedido* pedido);
void inicializar_diccionario_acciones();
void descansar_repartidor(t_pedido* pedido);
bool  misma_posicion (t_posicion* posicion1, t_posicion* posicion2);

#endif //PEDIDOS_APP
