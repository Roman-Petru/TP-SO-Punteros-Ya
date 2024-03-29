#ifndef GESTOR_CLIENTES_H_
#define GESTOR_CLIENTES_H_

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "../Utils/posicion.h"
#include "../Utils/protocolo.h"

typedef struct {
	char* id;
	t_posicion* posicion;
	t_list* pedidos;
	char* restaurante_seleccionado;
	char* IP;
	char* Puerto;
} t_cliente_conectado;

extern t_list* clientes_conectados;

void inicializar_gestor_clientes();
void finalizar_gestor_clientes();

int cliente_index(char* id_cliente);
void agregar_cliente(t_datos_cliente* datos);
void cliente_agregar_pedido(char* id_cliente, int id_pedido);
void cliente_remover_pedido(char* id_cliente, int id_pedido);
void cliente_seleccionar_restaurante(char* id_cliente, char* nombre_restaurante);
t_posicion* cliente_obtener_posicion(char* id_cliente);
void remover_cliente(char* id_cliente);
char* cliente_obtener_restaurante(char* id_cliente);

#endif /*GESTOR_CLIENTES_H_*/
