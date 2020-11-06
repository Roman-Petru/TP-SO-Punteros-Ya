#ifndef SERVIDOR_RED
#define SERVIDOR_RED

#include "socket.h"
#include "paquete.h"
#include "protocolo.h"
#include "cliente_red.h"
#include "serializacion.h"
#include "dictionary_int.h"
#include <commons/log.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


typedef struct
{
	int socket;
	pthread_t hilo_escucha;
	t_dictionary_int* diccionario_operaciones;
}t_servidor_red;

typedef struct
{
	t_servidor_red* servidor;
	int socket;
}t_cliente_servidor;

t_servidor_red* servidor_crear(char* ip, char* puerto);
void servidor_destruir(t_servidor_red* servidor);
void servidor_agregar_operacion(t_servidor_red* servidor, t_codigo_de_operacion codigo_operacion, void* operacion);

#endif /*SERVIDOR_RED*/
