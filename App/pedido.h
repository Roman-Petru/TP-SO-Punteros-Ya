#ifndef PEDIDO_H_
#define PEDIDO_H_

#include "stddef.h"
#include <pthread.h>
#include "../Utils/dictionary_int.h"
#include <semaphore.h>
#include <stdbool.h>
#include "repartidor.h"

extern t_dictionary_int* diccionario_acciones;

typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCKED,
	EXIT
} ESTADO_PCB;

typedef enum {
	IR_A_RESTAURANTE,
	IR_A_CLIENTE,
	ESPERAR_EN_RESTAURANTE
} INSTRUCCION_A_REALIZAR;

typedef struct {
	int id_pedido;
	t_repartidor* repartidor;
	t_posicion* posicion_de_restaurante;
	t_posicion* posicion_cliente;

	int ciclos_en_ready;
	float estimacion;
	bool esta_listo;

	ESTADO_PCB estado_pcb;
	INSTRUCCION_A_REALIZAR instruccion_a_realizar;

	pthread_t hilo;
	sem_t mutex;

} t_pedido;

typedef void (*t_accion)(t_pedido* pedido);

t_pedido* pedido_crear(int id, t_posicion* posicion_de_restaurante, t_posicion* posicion_cliente, bool resto_default);
bool pedido_es_mismo(t_pedido* pedido_A, t_pedido* pedido_B);
void inicializar_diccionario_acciones();
void descansar_repartidor(t_pedido* pedido);

t_pedido* crear_pedido_default(int id);

#endif /*PEDIDO_H_*/
