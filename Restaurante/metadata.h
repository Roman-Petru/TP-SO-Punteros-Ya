#ifndef METH_H_
#define METH_H_

#include "../Utils/protocolo.h"
#include <commons/string.h>
#include <stdbool.h>
#include "restaurante.h"
#include <pthread.h>

typedef struct{
	int id_pedido;
	int platos_total;
	int platos_listos;
	sem_t* sincronizador;
}t_platos_listos;

typedef struct {
	char* id;
	t_list* pedidos;
	t_cliente_red* cliente;
} t_cliente_conectado;

extern pthread_mutex_t mutex_pedidos;
void agendar_pedido(int id_pedido, t_datos_estado_pedido* estado_pedido);
void obtener_metadata();
bool obtener_recetas(t_list* platos);
bool existe_plato(char* nombre_plato_buscado);
void agregar_cliente(t_datos_cliente* datos);
void cliente_agregar_pedido(char* id_cliente, int id_pedido);
t_cliente_red* conseguir_cliente_pedido(int id_pedido);

#endif
