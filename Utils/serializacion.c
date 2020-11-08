#include "serializacion.h"

t_dictionary_int* diccionario_serializaciones;
t_dictionary_int* diccionario_deserializaciones;
t_dictionary_int* diccionario_destrucciones;
t_dictionary_int* diccionario_destruccion_respuestas;

// ===== Serializaciones ===== //
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

static t_buffer* serializar_int(void* datos)
{
	t_buffer* buffer = buffer_crear(sizeof(uint32_t));
	buffer_serializar_int(buffer, (uint32_t) datos);

	return buffer;
}

static t_buffer* serializar_posicion(void* datos)
{
	t_buffer* buffer = buffer_crear(sizeof(int)*2);
	t_posicion* posicion = datos;

	buffer_serializar_int(buffer, posicion->x);
	buffer_serializar_int(buffer, posicion->y);

	return buffer;
}

/*static t_buffer* serializar_datos_pedido(void* datos_void)
{
	t_datos_pedido* datos = datos_void;
	size_t tamanio = strlen(datos->restaurante);
	t_buffer* buffer = buffer_crear(sizeof(uint32_t)+sizeof(size_t)+tamanio);

	buffer_serializar_int(buffer, datos->id_pedido);
	buffer_serializar_string(buffer, datos->restaurante);

	return buffer;
}*/

static t_buffer* serializar_seleccion_restaurante(void* datos_void)
{
	t_datos_seleccion_restaurante* datos = datos_void;

	size_t tamanio = strlen(datos->restaurante);
	t_buffer* buffer = buffer_crear(sizeof(uint32_t)+sizeof(size_t)+tamanio);

	buffer_serializar_int(buffer, datos->id_cliente);
	buffer_serializar_string(buffer, datos->restaurante);

	buffer->desplazamiento = 0;

	return buffer;
}

static t_buffer* serializar_handshake_restaurante_app(void* datos_void)
{
	t_handshake_resto_app* datos = datos_void;

	size_t tamanio = strlen(datos->restaurante);

	t_buffer* buffer = buffer_crear(sizeof(uint32_t)+sizeof(size_t)+tamanio+sizeof(int)*2);

	buffer_serializar_int(buffer, datos->puerto);
	buffer_serializar_string(buffer, datos->restaurante);
	buffer_serializar_int(buffer, datos->posicion->x);
	buffer_serializar_int(buffer, datos->posicion->y);

	buffer->desplazamiento = 0;

	return buffer;
}

static t_buffer* serializar_sin_datos(void* datos)
{
	t_buffer* buffer = buffer_crear(0);
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

static void* deserializar_bool(t_buffer* buffer)
{
	return buffer_deserializar(buffer, sizeof(bool));
}

static int deserializar_int(t_buffer* buffer)
{
	return buffer_deserializar_int(buffer);
}

static void* deserializar_posicion(t_buffer* buffer)
{
	int x = buffer_deserializar_int(buffer);
	int y = buffer_deserializar_int(buffer);

	return posicion_crear(x, y);
}

/*static void* deserializar_datos_pedido(t_buffer* buffer)
{
	return crear_datos_pedido(buffer_deserializar_int(buffer), buffer_deserializar_string(buffer));
}*/

static void* deserializar_seleccion_restaurante(t_buffer* buffer)
{
	int id_cliente = buffer_deserializar_int(buffer);
	char* restaurante = buffer_deserializar_string(buffer);

	return crear_datos_seleccion_restaurante(id_cliente, restaurante);
}

static void* deserializar_handshake_restaurante_app(t_buffer* buffer)
{
	int puerto = buffer_deserializar_int(buffer);
	char* restaurante = buffer_deserializar_string(buffer);
	int x = buffer_deserializar_int(buffer);
	int y = buffer_deserializar_int(buffer);

	t_posicion* posicion = posicion_crear(x, y);

	return crear_datos_handshake_restaurante_app(puerto, restaurante, posicion);
}

static void* deserializar_sin_datos(t_buffer* buffer)
{
	return NULL;
}

// ===== Destrucciones =====
void destruir_lista_string(void* datos) { list_destroy_and_destroy_elements(datos, &free); }

void destruir_datos_pedido(void* datos_void)
{
	t_datos_pedido* datos = datos_void;
	free(datos->restaurante);
	free(datos);
}


static void sin_free() { }

// ===== Diccionario de Serializaciones =====
void diccionario_serializaciones_inicializar()
{
	diccionario_serializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_serializaciones, CONEXION_CLIENTE, &serializar_posicion);
	dictionary_int_put(diccionario_serializaciones, CONEXION_CLIENTE_RESPUESTA, &serializar_int);
	dictionary_int_put(diccionario_serializaciones, HANDSHAKE_RESTO_APP, &serializar_handshake_restaurante_app);
	dictionary_int_put(diccionario_serializaciones, HANDSHAKE_RESTO_APP_RESPUESTA, &serializar_bool);


	dictionary_int_put(diccionario_serializaciones, CONSULTAR_RESTAURANTES, &serializar_sin_datos);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_RESTAURANTES_RESPUESTA, &serializar_lista_string);
	dictionary_int_put(diccionario_serializaciones, SELECCIONAR_RESTAURANTE, &serializar_seleccion_restaurante);
	dictionary_int_put(diccionario_serializaciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_PLATOS, &serializar_string);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_PLATOS_RESPUESTA, &serializar_lista_string);
	dictionary_int_put(diccionario_serializaciones, CREAR_PEDIDO, &serializar_int);
	dictionary_int_put(diccionario_serializaciones, CREAR_PEDIDO_RESPUESTA, &serializar_int);
}

void diccionario_serializaciones_destruir()
{
	dictionary_int_destroy(diccionario_serializaciones);
}

// ===== Diccionario de Deserializaciones =====
void diccionario_deserializaciones_inicializar()
{
	diccionario_deserializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_deserializaciones, CONEXION_CLIENTE, &deserializar_posicion);
	dictionary_int_put(diccionario_deserializaciones, CONEXION_CLIENTE_RESPUESTA, &deserializar_int);
	dictionary_int_put(diccionario_deserializaciones, HANDSHAKE_RESTO_APP, &deserializar_handshake_restaurante_app);
	dictionary_int_put(diccionario_deserializaciones, HANDSHAKE_RESTO_APP_RESPUESTA, &deserializar_bool);

	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_RESTAURANTES, &deserializar_sin_datos);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_RESTAURANTES_RESPUESTA, &deserializar_lista_string);
	dictionary_int_put(diccionario_deserializaciones, SELECCIONAR_RESTAURANTE, &deserializar_seleccion_restaurante);
	dictionary_int_put(diccionario_deserializaciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_PLATOS, &deserializar_string);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_PLATOS_RESPUESTA, &deserializar_lista_string);
	dictionary_int_put(diccionario_deserializaciones, CREAR_PEDIDO, &deserializar_int);
	dictionary_int_put(diccionario_deserializaciones, CREAR_PEDIDO_RESPUESTA, &deserializar_int);
}

// ===== Diccionario de Destrucciones =====
void diccionario_destrucciones_inicializar()
{
	diccionario_destrucciones = dictionary_int_create();

	dictionary_int_put(diccionario_destrucciones, CONEXION_CLIENTE, &free);
	dictionary_int_put(diccionario_destrucciones, CONEXION_CLIENTE_RESPUESTA, &free);
	dictionary_int_put(diccionario_destrucciones, HANDSHAKE_RESTO_APP, &sin_free); // VER Q FREE
	dictionary_int_put(diccionario_destrucciones, HANDSHAKE_RESTO_APP_RESPUESTA, &free);


	dictionary_int_put(diccionario_destrucciones, CONSULTAR_RESTAURANTES, &sin_free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_RESTAURANTES_RESPUESTA, &destruir_lista_string);
	dictionary_int_put(diccionario_destrucciones, SELECCIONAR_RESTAURANTE, &sin_free); // VER Q FREE
	dictionary_int_put(diccionario_destrucciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_PLATOS, &free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_PLATOS_RESPUESTA, &destruir_lista_string);
	dictionary_int_put(diccionario_destrucciones, CREAR_PEDIDO, &sin_free);
	dictionary_int_put(diccionario_destrucciones, CREAR_PEDIDO_RESPUESTA, &sin_free);
}

void destruir(t_codigo_de_operacion codigo_de_operacion, void* datos)
{
	if(datos!=NULL)
		((t_destructor) dictionary_int_get(diccionario_destrucciones, codigo_de_operacion))(datos);
}

// ===== Diccionario de Destruccion Respuestas =====
void inicializar_diccionario_destruccion_respuestas()
{
	diccionario_destruccion_respuestas = dictionary_int_create();

	dictionary_int_put(diccionario_destruccion_respuestas, CONSULTAR_RESTAURANTES_RESPUESTA, &list_destroy);
	dictionary_int_put(diccionario_destruccion_respuestas, CONSULTAR_PLATOS_RESPUESTA, &destruir_lista_string);
}

void destruir_datos_respuesta(t_codigo_de_operacion codigo_de_operacion, void* datos)
{
	if(datos!=NULL)
		((t_destructor) dictionary_int_get(diccionario_destruccion_respuestas, codigo_de_operacion))(datos);
}

// ===== Serializacion =====
void serializacion_inicializar()
{
	diccionario_serializaciones_inicializar();
	diccionario_deserializaciones_inicializar();
	diccionario_destrucciones_inicializar();
	inicializar_diccionario_destruccion_respuestas();
}

void serializacion_finalizar()
{
	dictionary_int_destroy(diccionario_serializaciones);
	dictionary_int_destroy(diccionario_deserializaciones);
	dictionary_int_destroy(diccionario_destrucciones);
	dictionary_int_destroy(diccionario_destruccion_respuestas);
}

