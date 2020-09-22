#include "serializacion.h"
#include "protocolo.h"
#include <string.h>

t_dictionary_int* diccionario_serializaciones;
t_dictionary_int* diccionario_deserializaciones;

// ===== Serializaciones =====
static t_buffer* serializar_string(void* datos)
{
	t_buffer* buffer = buffer_crear(sizeof(size_t) + strlen(datos));
	buffer_serializar_string(buffer, datos);

	return buffer;
}

static t_buffer* serializar_consultar_armas(void* datos)
{
	t_list* armas = datos;
	size_t tamanio = sizeof(size_t)*list_size(armas);
	void sumar_len_arma(void* arma)	{ tamanio = tamanio + strlen(arma); }
	list_iterate(armas, &sumar_len_arma);

	t_buffer* buffer = buffer_crear(tamanio);
	void serializar_arma(void* arma) { buffer_serializar_string(buffer, arma); }
	list_iterate(armas, &serializar_arma);

	return buffer;
}

// ===== Deserializaciones =====
static void* deserializar_string(t_buffer* buffer)
{
	return buffer_deserializar_string(buffer);
}

static void* deserializar_consultar_armas(t_buffer* buffer)
{
	t_list* armas = list_create();
	while(!buffer_leido(buffer))
		list_add(armas, buffer_deserializar_string(buffer));

	return armas;
}

// ===== Diccionario de Serializaciones =====
void diccionario_serializaciones_inicializar()
{
	diccionario_serializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_serializaciones, LOGGUEAR, &serializar_string);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_ARMAS, &serializar_consultar_armas);
}

void diccionario_serializaciones_destruir()
{
	dictionary_int_destroy(diccionario_serializaciones);
}

// ===== Diccionario de Deserializaciones =====
void diccionario_deserializaciones_inicializar()
{
	diccionario_deserializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_deserializaciones, LOGGUEAR, &deserializar_string);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_ARMAS, &deserializar_consultar_armas);
}

void diccionario_deserializaciones_destruir()
{
	dictionary_int_destroy(diccionario_deserializaciones);
}
