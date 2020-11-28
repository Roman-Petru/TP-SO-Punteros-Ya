#include "servidor_red.h"

// ===== Cliente de Servidor =====
static t_cliente_servidor esperar_cliente(t_servidor_red* servidor)
{
	int socket = socket_escucha_esperar_mensaje(servidor->socket);
	t_cliente_servidor cliente;// = malloc(sizeof(t_cliente_servidor));
	cliente.servidor = servidor;
	cliente.socket = socket;

	return cliente;
}

/*free(t_cliente_servidor)*/

static bool es_cliente_valido(t_cliente_servidor* cliente) { return cliente->socket > 0; }

// ===== Servidor =====
static t_respuesta* servidor_procesar_paquete(t_servidor_red* servidor, t_paquete* paquete)
{
	t_respuesta* respuesta;
	if(paquete_tiene_datos(paquete))
	{
		void* datos = paquete_desempaquetar(paquete);
		respuesta =((t_operacion_servidor) dictionary_int_get(servidor->diccionario_operaciones, paquete->codigo_operacion))(datos);
		destruir(paquete->codigo_operacion, datos);
	}
	else
		respuesta = ((t_operacion_servidor_simple) dictionary_int_get(servidor->diccionario_operaciones, paquete->codigo_operacion))();

	return respuesta;
}

static void servidor_enviar_respuesta(int socket_cliente, t_respuesta* respuesta)
{
	t_buffer* buffer = buffer_crear_con_datos(respuesta->codigo_operacion, respuesta->datos);
	t_paquete* paquete_respuesta = paquete_crear(respuesta->codigo_operacion, buffer);
	paquete_enviar(paquete_respuesta, socket_cliente);
	paquete_destruir(paquete_respuesta);
}

static void atender_cliente(t_cliente_servidor* cliente)
{
	t_paquete* paquete = paquete_recibir(cliente->socket);
	if(paquete == NULL)
		return;

	t_respuesta* respuesta = servidor_procesar_paquete(cliente->servidor, paquete);
	paquete_destruir(paquete);
	servidor_enviar_respuesta(cliente->socket, respuesta);
	respuesta_destruir(respuesta);
}

static void servidor_atender_cliente(t_servidor_red* servidor, t_cliente_servidor* cliente)
{
	pthread_t asistente_cliente;

	pthread_create(&asistente_cliente, NULL, (void*) atender_cliente, cliente);
	pthread_detach(asistente_cliente);
}

static void servidor_escuchar_mensajes(t_servidor_red* servidor)
{
	t_cliente_servidor cliente;
	while(true)
	{
		cliente = esperar_cliente(servidor);
		if(!es_cliente_valido(&cliente))
			break;

		servidor_atender_cliente(servidor, &cliente);
	}
}

// ===== Servidor =====
t_servidor_red* servidor_crear(char* ip, char* puerto)
{
	int socket = socket_escucha_crear(ip, puerto);

	t_servidor_red* servidor = malloc(sizeof(t_servidor_red));
	servidor->socket = socket;
	servidor->diccionario_operaciones = dictionary_int_create();

	pthread_create(&(servidor->hilo_escucha), NULL, (void*) servidor_escuchar_mensajes, servidor);
	pthread_detach(servidor->hilo_escucha);

	return servidor;
}

void servidor_agregar_operacion(t_servidor_red* servidor, t_codigo_de_operacion codigo_operacion, void* operacion)
{
	dictionary_int_put(servidor->diccionario_operaciones, codigo_operacion, operacion);
}

void servidor_destruir(t_servidor_red* servidor)
{
	socket_cerrar(servidor->socket);
	dictionary_int_destroy(servidor->diccionario_operaciones);
	free(servidor);
}
