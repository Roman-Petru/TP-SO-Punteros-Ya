#ifndef CLIENTE_RED_H_
#define CLIENTE_RED_H_

#include "buffer.h"
#include "socket.h"
#include "paquete.h"
#include "protocolo.h"
#include "serializacion.h"
#include "dictionary_int.h"
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct
{
	char* ip;
	char* puerto;
	int socket;
}t_cliente_red;

t_cliente_red* cliente_crear(char* ip, char* puerto);
void cliente_destruir(t_cliente_red* cliente);
void cliente_cambiar_servidor(t_cliente_red* cliente, char* servidor);
void* cliente_enviar_mensaje(t_cliente_red* cliente, char* servidor, t_codigo_de_operacion codigo_operacion, void* datos);
void cliente_agregar_operacion(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* operacion);

#endif /*CLIENTE_RED_H_*/
