#ifndef PEDIDO_H_
#define PEDIDO_H_

#include "stddef.h"
#include "repartidor.h"
#include "gestor_pedidos.h"
#include "gestor_clientes.h"
#include "gestor_restaurantes.h"
#include "../Utils/protocolo.h"
#include "../Utils/dictionary_int.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

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
	int pcb_id;
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

t_pedido* pedido_crear(int id_pedido);
void recibir_pedidos_default(int cantidad);

void pedido_destruir(t_pedido* pedido);
bool pedido_es_mismo(t_pedido* pedido_A, t_pedido* pedido_B);
void pedido_actualizar_estado(int id_pedido, t_estado_pedido estado);
void inicializar_diccionario_acciones();
float pedido_estimacion(t_pedido* pedido);

#endif /*PEDIDO_H_*/
