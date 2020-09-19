#include "servidor_red.h"
#include "cliente_red.h"
#include "socket.h"
#include "paquete.h"
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>

static t_codigo_de_operacion servidor_procesar_paquete(t_servidor_red* servidor, t_paquete* paquete)
{
	if(paquete->buffer->tamanio == 0)
	{
		((t_operacion_respuesta) dictionary_int_get(servidor->diccionario_operaciones, paquete->codigo_operacion))();
		return OPERACION_OK;
	}

	void* datos = paquete_desempaquetar(paquete);
	t_codigo_de_operacion operacion_respuesta = ((t_operacion) dictionary_int_get(servidor->diccionario_operaciones, paquete->codigo_operacion))(datos);
	return operacion_respuesta;
}

static void servidor_enviar_respuesta(int socket_cliente, t_codigo_de_operacion codigo_operacion)
{
	t_paquete* paquete_respuesta = paquete_crear(codigo_operacion, buffer_crear(0));
	paquete_enviar(paquete_respuesta, socket_cliente);
	paquete_destruir(paquete_respuesta);
}

static void servidor_escuchar_mensajes(t_servidor_red* servidor)
{
	int socket_cliente;
	while(true)
	{
		socket_cliente = socket_escucha_esperar_mensaje(servidor->socket);
		if(socket_cliente == -1)
			break;

		t_paquete* paquete = paquete_recibir(socket_cliente);
		if(paquete == NULL)
			break;

		t_codigo_de_operacion codigo_respuesta = servidor_procesar_paquete(servidor, paquete);
		paquete_destruir(paquete);
		servidor_enviar_respuesta(socket_cliente, codigo_respuesta);
	}
}

static void servidor_crear_hilo_escucha(t_servidor_red* servidor)
{
	servidor->hilo_escucha = malloc(sizeof(pthread_t));
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(servidor->hilo_escucha, &attr, (void*)servidor_escuchar_mensajes, servidor);
}

// ===== Servidor =====
t_servidor_red* servidor_crear(char* ip, char* puerto)
{
	int socket = socket_escucha_crear(ip, puerto);

	t_servidor_red* servidor = malloc(sizeof(t_servidor_red));
	servidor->socket = socket;
	servidor->diccionario_operaciones = dictionary_int_create();
	servidor_crear_hilo_escucha(servidor);

	return servidor;
}

void servidor_agregar_operacion(t_servidor_red* servidor, t_codigo_de_operacion codigo_operacion, t_operacion operacion)
{
	dictionary_int_put(servidor->diccionario_operaciones, codigo_operacion, operacion);
}

void servidor_destruir(t_servidor_red* servidor)
{
	socket_cerrar(servidor->socket);
	pthread_join(*(servidor->hilo_escucha), NULL);
	free(servidor->hilo_escucha);
	dictionary_int_destroy(servidor->diccionario_operaciones);
	free(servidor);
}


