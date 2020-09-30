#include "serializacion.h"
#include "protocolo.h"
#include <string.h>
#include <stdlib.h>

t_dictionary_int* diccionario_serializaciones;
t_dictionary_int* diccionario_deserializaciones;
t_dictionary_int* diccionario_destrucciones;

// ===== Serializaciones =====
static t_buffer* serializar_string(void* datos)
{
	t_buffer* buffer = buffer_crear(sizeof(size_t) + strlen(datos));
	buffer_serializar_string(buffer, datos);

	return buffer;
}

static t_buffer* serializar_lista_string(void* datos)
{
	t_list* lista = datos;
	size_t tamanio = sizeof(size_t)*list_size(lista);
	void sumar_len_string(void* string)	{ tamanio = tamanio + strlen(string); }
	list_iterate(lista, &sumar_len_string);

	t_buffer* buffer = buffer_crear(tamanio);
	void serializar_lista(void* string) { buffer_serializar_string(buffer, string); }
	list_iterate(lista, &serializar_lista);

	return buffer;
}

static t_buffer* serializar_bool(void* datos)
{
	t_buffer* buffer = buffer_crear(sizeof(bool));
	buffer_serializar(buffer, &datos, sizeof(bool));

	return buffer;
}

// ===== Deserializaciones =====
static void* deserializar_string(t_buffer* buffer)
{
	return buffer_deserializar_string(buffer);
}

static void* deserializar_lista_string(t_buffer* buffer)
{
	t_list* lista = list_create();
	while(!buffer_leido(buffer))
		list_add(lista, buffer_deserializar_string(buffer));

	return lista;
}

// ===== Destrucciones =====
static void destruir_lista_string(void* datos) { list_destroy_and_destroy_elements(datos, &free); }

static void* deserializar_bool(t_buffer* buffer)
{
	return buffer_deserializar(buffer, sizeof(bool));
}

// ===== Diccionario de Serializaciones =====
void diccionario_serializaciones_inicializar()
{
	diccionario_serializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_serializaciones, CONSULTAR_RESTAURANTES, &serializar_lista_string);
	dictionary_int_put(diccionario_serializaciones, SELECCIONAR_RESTAURANTE, &serializar_string);
	dictionary_int_put(diccionario_serializaciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_PLATOS, &serializar_string);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_PLATOS_RESPUESTA, &serializar_lista_string);
}

void diccionario_serializaciones_destruir()
{
	dictionary_int_destroy(diccionario_serializaciones);
}

// ===== Diccionario de Deserializaciones =====
void diccionario_deserializaciones_inicializar()
{
	diccionario_deserializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_RESTAURANTES, &deserializar_lista_string);
	dictionary_int_put(diccionario_deserializaciones, SELECCIONAR_RESTAURANTE, &deserializar_string);
	dictionary_int_put(diccionario_deserializaciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_PLATOS, &deserializar_string);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_PLATOS_RESPUESTA, &deserializar_lista_string);
}

//static void sin_free() {}
static void si_free(void* datos) {if(datos != NULL) free(datos);}

// ===== Diccionario de Destrucciones =====
void diccionario_destrucciones_inicializar()
{
	diccionario_destrucciones = dictionary_int_create();

	dictionary_int_put(diccionario_destrucciones, CONSULTAR_RESTAURANTES, &destruir_lista_string);
	dictionary_int_put(diccionario_destrucciones, SELECCIONAR_RESTAURANTE, &si_free);
	dictionary_int_put(diccionario_destrucciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &si_free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_PLATOS, &si_free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_PLATOS_RESPUESTA, &destruir_lista_string);
}

void destruir(t_codigo_de_operacion codigo_de_operacion, void* datos)
{
	if(datos!=NULL)
		((t_destructor) dictionary_int_get(diccionario_destrucciones, codigo_de_operacion))(datos);
}

// ===== Serializacion =====
void serializacion_inicializar()
{
	diccionario_serializaciones_inicializar();
	diccionario_deserializaciones_inicializar();
	diccionario_serializaciones_destruir();
}

void serializacion_finalizar()
{
	dictionary_int_destroy(diccionario_serializaciones);
	dictionary_int_destroy(diccionario_deserializaciones);
	dictionary_int_destroy(diccionario_destrucciones);
}

