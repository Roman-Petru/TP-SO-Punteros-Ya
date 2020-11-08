#ifndef GESTOR_CLIENTES_H_
#define GESTOR_CLIENTES_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include "../Utils/posicion.h"

extern int id_cliente;

typedef struct {
	int id;
	t_posicion* posicion;
	t_list* pedidos;
	char* restaurante_seleccionado;
} t_cliente_conectado;

void inicializar_gestor_clientes();
void agregar_cliente(t_posicion* posicion);
void cliente_agregar_pedido(int id_cliente, int id_pedido);
void cliente_remover_pedido(int id_cliente, int id_pedido);
void cliente_seleccionar_restaurante(int id_cliente, char* nombre_restaurante);
t_posicion* cliente_obtener_posicion(int id_cliente);
void remover_cliente(int id_cliente);
char* cliente_obtener_restaurante(int id_cliente);

#endif /*GESTOR_CLIENTES_H_*/
