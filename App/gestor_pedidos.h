#ifndef GESTOR_PEDIDOS_H_
#define GESTOR_PEDIDOS_H_

#include "gestor_clientes.h"
#include <commons/collections/list.h>
#include <commons/config.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
	int id;
	char* id_cliente;
	char* restaurante;
} t_pedido_pendiente;

void inicializar_gestor_pedidos();
void finalizar_gestor_pedidos();

void vincular_pedido(int id_pedido, char* id_cliente, char* restaurante);
char* pedido_obtener_restaurante(int id_pedido);
char* pedido_obtener_cliente(int id_pedido);
void remover_pedido(int id_pedido);
int generar_id_pedido();

#endif /*GESTOR_PEDIDOS_H_*/
