#include "servidor_red.h"
#include "cliente_red.h"
#include "socket.h"
#include "paquete.h"
#include "protocolo.h"
#include "serializacion.h"
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>

static t_respuesta* servidor_procesar_paquete(t_servidor_red* servidor, t_paquete* paquete)
{
	if(paquete_tiene_datos(paquete))
		return ((t_operacion_servidor) dictionary_int_get(servidor->diccionario_operaciones, paquete->codigo_operacion))(paquete_desempaquetar(paquete));
	else
		return ((t_operacion_servidor_simple) dictionary_int_get(servidor->diccionario_operaciones, paquete->codigo_operacion))();
}

static void servidor_enviar_respuesta(int socket_cliente, t_respuesta* respuesta)
{
	t_buffer* buffer = buffer_crear_con_datos(respuesta->codigo_operacion, respuesta->datos);
	t_paquete* paquete_respuesta = paquete_crear(respuesta->codigo_operacion, buffer);
	paquete_enviar(paquete_respuesta, socket_cliente);
	paquete_destruir(paquete_respuesta);
	//destruir respuesta
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

		t_respuesta* respuesta = servidor_procesar_paquete(servidor, paquete);
		paquete_destruir(paquete);
		servidor_enviar_respuesta(socket_cliente, respuesta);
		respuesta_destruir(respuesta);
	}
	pthread_exit(NULL);
}
// ===== Servidor =====
t_servidor_red* servidor_crear(char* ip, char* puerto)
{
	int socket = socket_escucha_crear(ip, puerto);

	t_servidor_red* servidor = malloc(sizeof(t_servidor_red));
	servidor->socket = socket;
	servidor->diccionario_operaciones = dictionary_int_create();
	pthread_create(&(servidor->hilo_escucha), NULL, (void*) servidor_escuchar_mensajes, servidor);

	return servidor;
}

void servidor_agregar_operacion(t_servidor_red* servidor, t_codigo_de_operacion codigo_operacion, void* operacion)
{
	dictionary_int_put(servidor->diccionario_operaciones, codigo_operacion, operacion);
}

void servidor_destruir(t_servidor_red* servidor)
{
	socket_cerrar(servidor->socket);
	pthread_join(servidor->hilo_escucha, NULL);
	dictionary_int_destroy(servidor->diccionario_operaciones);
	free(servidor);

}


