#include "protocolo.h"
#include "buffer.h"
#include <string.h>
#include <commons/log.h>

t_dictionary_int* diccionario_serializaciones;
t_dictionary_int* diccionario_deserializaciones;

// ===== Operaciones =====

// ===== Operaciones Respuesta =====
void handshake_ok()
{
	t_log* logger = log_create("cliente.log", "Cliente", true, LOG_LEVEL_INFO);
	log_info(logger, "HANDSHAKE OK.");
	log_destroy(logger);
}

void operacion_ok()
{
	t_log* logger = log_create("cliente.log", "Cliente", true, LOG_LEVEL_INFO);
	log_info(logger, "OPERACION OK.");
	log_destroy(logger);
}

// ===== Serializaciones =====
static t_buffer* serializar_string(void* datos)
{
	t_buffer* buffer = buffer_crear(sizeof(size_t) + strlen(datos));
	buffer_serializar_string(buffer, datos);

	return buffer;
}

// ===== Deserializaciones =====
static void* deserializar_string(t_buffer* buffer)
{
	return buffer_deserializar_string(buffer);
}

// ===== Diccionario de Serializaciones =====
void diccionario_serializaciones_inicializar()
{
	diccionario_serializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_serializaciones, CONSOLA, &serializar_string);
}

void diccionario_serializaciones_destruir()
{
	dictionary_int_destroy(diccionario_serializaciones);
}

// ===== Diccionario de Deserializaciones =====
void diccionario_deserializaciones_inicializar()
{
	diccionario_deserializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_deserializaciones, CONSOLA, &deserializar_string);
}

void diccionario_deserializaciones_destruir()
{
	dictionary_int_destroy(diccionario_deserializaciones);
}
