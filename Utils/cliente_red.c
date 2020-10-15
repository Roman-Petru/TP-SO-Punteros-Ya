#include "cliente_red.h"

// ===== Cliente =====
t_cliente_red* cliente_crear(t_config* config, char* servidor)
{
	t_cliente_red* cliente = malloc(sizeof(t_cliente_red));
	cliente->config =  config;
	cliente->servidor = malloc(strlen(cliente->servidor)+1);
	strcpy(cliente->servidor, servidor);
	cliente->diccionario_operaciones = dictionary_int_create();

	return cliente;
}

char* cliente_ip(t_cliente_red* cliente)
{
	char* key = malloc(3+strlen(cliente->servidor)+1);
	sprintf(key, "IP_%s", cliente->servidor);
	char* ip = config_get_string_value(cliente->config, key);
	free(key);
	return ip;
}

char* cliente_puerto(t_cliente_red* cliente)
{
	char* key = malloc(7+strlen(cliente->servidor)+1);
	sprintf(key, "PUERTO_%s", cliente->servidor);
	char* puerto = config_get_string_value(cliente->config, key);
	free(key);
	return puerto;
}

void cliente_destruir(t_cliente_red* cliente)
{
	dictionary_int_destroy(cliente->diccionario_operaciones);
	free(cliente->servidor);
	free(cliente);
}

void cliente_cambiar_servidor(t_cliente_red* cliente, char* servidor)
{
	free(cliente->servidor);
	cliente->servidor = malloc(strlen(cliente->servidor)+1);
	strcpy(cliente->servidor, servidor);
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
	{
		void* datos = paquete_desempaquetar(paquete);
		((t_operacion_cliente) dictionary_int_get(cliente->diccionario_operaciones, paquete->codigo_operacion))(datos);
		destruir(paquete->codigo_operacion, datos);
	}
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
	cliente->socket = socket_crear(cliente_ip(cliente), cliente_puerto(cliente));
	t_buffer* buffer = buffer_crear_con_datos(codigo_operacion, datos);
	t_paquete* paquete = paquete_crear(codigo_operacion, buffer);
	paquete_enviar(paquete, cliente->socket);
	paquete_destruir(paquete);

	//cliente_crear_hilo_recibir_respuesta(cliente);
	cliente_recibir_respuesta(cliente);
}
