#include "cliente_red.h"
#include "buffer.h"
#include "socket.h"
#include "paquete.h"
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

// ===== Cliente =====
t_cliente_red* cliente_crear(char* ip, char* puerto)
{
	t_cliente_red* cliente = malloc(sizeof(t_cliente_red));
	cliente->ip = malloc(strlen(ip)+1);
	strcpy(cliente->ip, ip);
	cliente->puerto = malloc(strlen(ip)+1);
	strcpy(cliente->puerto, puerto);
	cliente->diccionario_operaciones = dictionary_int_create();
	dictionary_int_put(cliente->diccionario_operaciones, OPERACION_OK, &operacion_ok);
	//cliente_enviar_mensaje(cliente, HANDSHAKE_OK, NULL);

	return cliente;
}

void cliente_destruir(t_cliente_red* cliente)
{
	dictionary_int_destroy(cliente->diccionario_operaciones);
	free(cliente->puerto);
	free(cliente->ip);
	free(cliente);
}

void cliente_agregar_operacion(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* operacion)
{
	dictionary_int_put(cliente->diccionario_operaciones, codigo_operacion, operacion);
}

// ===== Recibir Respuesta =====
static void cliente_recibir_respuesta(t_cliente_red* cliente)
{
	t_paquete* paquete = paquete_recibir(cliente->socket);
	if(paquete_tiene_datos(paquete))
		((t_operacion_cliente) dictionary_int_get(cliente->diccionario_operaciones, paquete->codigo_operacion))(paquete_desempaquetar(paquete));
	else
		((t_operacion_cliente_simple) dictionary_int_get(cliente->diccionario_operaciones, paquete->codigo_operacion))();
	paquete_destruir(paquete);
	socket_cerrar(cliente->socket);
}

/*static void cliente_crear_hilo_recibir_respuesta(t_cliente_red* cliente)
{
	cliente->hilo_escucha = malloc(sizeof(pthread_t));
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(cliente->hilo_escucha, &attr, (void*)&cliente_recibir_respuesta, cliente);
}*/

// ===== Enviar Mensaje =====
void cliente_enviar_mensaje(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* datos)
{
	cliente->socket = socket_crear(cliente->ip, cliente->puerto);
	t_buffer* buffer = buffer_crear_con_datos(codigo_operacion, datos);
	t_paquete* paquete = paquete_crear(codigo_operacion, buffer);
	paquete_enviar(paquete, cliente->socket);
	paquete_destruir(paquete);

	//cliente_crear_hilo_recibir_respuesta(cliente);
	cliente_recibir_respuesta(cliente);
}
