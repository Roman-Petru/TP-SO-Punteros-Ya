#ifndef GESTOR_CLIENTES_H_
#define GESTOR_CLIENTES_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include "../Utils/posicion.h"
#include "../Utils/protocolo.h"

typedef struct {
	char* id;
	t_posicion* posicion;
	t_list* pedidos;
	char* restaurante_seleccionado;
} t_cliente_conectado;

void inicializar_gestor_clientes();

void agregar_cliente(t_datos_cliente* datos);
void cliente_agregar_pedido(char* id_cliente, int id_pedido);
void cliente_remover_pedido(char* id_cliente, int id_pedido);
void cliente_seleccionar_restaurante(char* id_cliente, char* nombre_restaurante);
t_posicion* cliente_obtener_posicion(char* id_cliente);
void remover_cliente(char* id_cliente);
char* cliente_obtener_restaurante(char* id_cliente);

#endif /*GESTOR_CLIENTES_H_*/
