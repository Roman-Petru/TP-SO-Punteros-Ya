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

extern pthread_mutex_t mutex_pedidos;
void agendar_pedido(int id_pedido, t_datos_estado_pedido* estado_pedido);
void obtener_metadata();
bool obtener_recetas(t_list* platos);
bool existe_plato(char* nombre_plato_buscado);

#endif
