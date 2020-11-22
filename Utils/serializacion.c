#include "serializacion.h"

t_dictionary_int* diccionario_serializaciones;
t_dictionary_int* diccionario_deserializaciones;
t_dictionary_int* diccionario_destrucciones;
t_dictionary_int* diccionario_destruccion_respuestas;

static size_t tamanio_lista_string(t_list* strings)
{
	size_t tamanio= 0;

	void sumar_tamanio_string(void* string)	{ tamanio = tamanio + strlen(string); }
	list_iterate(strings, &sumar_tamanio_string);

	return tamanio;
}

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

static t_buffer* serializar_modulo(void* datos)
{
	t_buffer* buffer = buffer_crear(sizeof(t_modulo));
	buffer_serializar(buffer, &datos, sizeof(t_modulo));

	return buffer;
}

/*static t_buffer* serializar_posicion(void* datos)
{
	t_buffer* buffer = buffer_crear(sizeof(uint32_t)*2);
	t_posicion* posicion = datos;

	buffer_serializar_int(buffer, posicion->x);
	buffer_serializar_int(buffer, posicion->y);

	return buffer;
}*/

static t_buffer* serializar_datos_cliente(void* datos_void)
{
	t_datos_cliente* datos = datos_void;
	t_buffer* buffer = buffer_crear(sizeof(uint32_t)*3 + strlen(datos->id_cliente));

	buffer_serializar_string(buffer, datos->id_cliente);
	buffer_serializar_int(buffer, datos->posicion->x);
	buffer_serializar_int(buffer, datos->posicion->y);

	return buffer;
}

static t_buffer* serializar_datos_pedido(void* datos_void)
{
	t_datos_pedido* datos = datos_void;
	size_t tamanio = strlen(datos->restaurante);
	t_buffer* buffer = buffer_crear(sizeof(uint32_t)+sizeof(size_t)+tamanio);

	buffer_serializar_int(buffer, datos->id_pedido);
	buffer_serializar_string(buffer, datos->restaurante);

	return buffer;
}

static t_buffer* serializar_seleccion_restaurante(void* datos_void)
{
	t_datos_seleccion_restaurante* datos = datos_void;

	size_t tamanio = strlen(datos->id_cliente) + strlen(datos->restaurante);

	t_buffer* buffer = buffer_crear(sizeof(size_t)*2+tamanio);

	buffer_serializar_string(buffer, datos->id_cliente);
	buffer_serializar_string(buffer, datos->restaurante);

	return buffer;
}

static t_buffer* serializar_guardar_plato(void* datos_void)
{
	t_guardar_plato* datos = datos_void;

	size_t tamanio_comida= strlen(datos->comida);
	size_t tamanio_restaurante= strlen(datos->restaurante);

	t_buffer* buffer = buffer_crear(sizeof(uint32_t)*4+tamanio_comida+tamanio_restaurante);

	buffer_serializar_int(buffer, datos->id_pedido);
	buffer_serializar_int(buffer, datos->cantidad);
	buffer_serializar_string(buffer, datos->comida);
	buffer_serializar_string(buffer, datos->restaurante);

	return buffer;
}

static t_buffer* serializar_agregar_plato(void* datos_void)
{
	t_agregar_plato* datos = datos_void;
	t_buffer* buffer = buffer_crear(sizeof(uint32_t)*2+strlen(datos->plato));

	buffer_serializar_int(buffer, datos->id_pedido);
	buffer_serializar_string(buffer, datos->plato);

	return buffer;
}


static t_buffer* serializar_consultar_pedido(void* datos_void)
{
	t_consultar_pedido* datos = datos_void;
	uint32_t cantidad_platos = list_size(datos->platos);


	char* nombre_plato(void* plato) { return ((t_datos_estado_comida*)plato)->comida;}
	t_list* lista_mapeada = list_map(datos->platos, (void*) &nombre_plato);

	t_buffer* buffer = buffer_crear(strlen(datos->restaurante)+sizeof(uint32_t)+sizeof(t_estado_pedido) + sizeof(uint32_t) + sizeof(uint32_t)*cantidad_platos*3 + tamanio_lista_string(lista_mapeada));
	//destruir_lista_string(lista_mapeada);

	buffer_serializar_string(buffer, datos->restaurante);
	buffer_serializar(buffer, &datos->estado, sizeof(t_estado_pedido));
	buffer_serializar_int(buffer, cantidad_platos);

	for (int i=0; i<cantidad_platos; i++)
		{
		t_datos_estado_comida* datos_estado_comida =  list_get(datos->platos, i);
		buffer_serializar_int(buffer, datos_estado_comida->cant_lista);
		buffer_serializar_int(buffer, datos_estado_comida->cant_total);

		buffer_serializar_string(buffer, datos_estado_comida->comida);
		}

	return buffer;
}

static t_buffer* serializar_estado_pedido(void* datos_void)
{
	t_datos_estado_pedido* datos = datos_void;
	uint32_t cantidad_platos = list_size(datos->platos);

	char* nombre_plato(void* plato) { return ((t_datos_estado_comida*)plato)->comida;}
	t_list* lista_mapeada = list_map(datos->platos, (void*) &nombre_plato);

	t_buffer* buffer = buffer_crear(sizeof(t_estado_pedido) + sizeof(uint32_t) + sizeof(uint32_t)*cantidad_platos*3 + tamanio_lista_string(lista_mapeada));
	//destruir_lista_string(lista_mapeada);

	buffer_serializar(buffer, &datos->estado, sizeof(t_estado_pedido));
	buffer_serializar_int(buffer, cantidad_platos);

	for (int i=0; i<cantidad_platos; i++)
		{
		t_datos_estado_comida* datos_estado_comida =  list_get(datos->platos, i);
		buffer_serializar_int(buffer, datos_estado_comida->cant_lista);
		buffer_serializar_int(buffer, datos_estado_comida->cant_total);

		buffer_serializar_string(buffer, datos_estado_comida->comida);
		}

	return buffer;
}

static t_buffer* serializar_obtener_receta(void* datos_void)
{
	t_obtener_receta* datos = datos_void;
	uint32_t cantidad_pasos = list_size(datos->pasos);

	char* nombre_paso(void* paso) { return ((t_paso*)paso)->operacion;}
	t_list* lista_mapeada = list_map(datos->pasos, (void*) &nombre_paso);

	t_buffer* buffer = buffer_crear(sizeof(uint32_t) + sizeof(uint32_t)*cantidad_pasos*2 + tamanio_lista_string(lista_mapeada));

	buffer_serializar_int(buffer, cantidad_pasos);

	for (int i=0; i<cantidad_pasos; i++)
		{
		t_paso* datos_paso =  list_get(datos->pasos, i);
		buffer_serializar_string(buffer, datos_paso->operacion);
		buffer_serializar_int(buffer, datos_paso->ciclos);
		}

	return buffer;
}

static t_buffer* serializar_obtener_restaurante(void* datos_void)
{
	t_obtener_restaurante* datos = datos_void;
	uint32_t cant_lista_afinidades = list_size(datos->lista_afinidades);
	uint32_t cant_lista_precios = list_size(datos->lista_precios);

	char* nombre_precio(void* precio) { return ((t_precio*)precio)->nombre_plato;}
	t_list* lista_mapeada = list_map(datos->lista_precios, (void*) &nombre_precio);


	t_buffer* buffer = buffer_crear(sizeof(uint32_t)*7+sizeof(uint32_t)*cant_lista_afinidades+sizeof(uint32_t)*cant_lista_precios*2+tamanio_lista_string(datos->lista_afinidades)+tamanio_lista_string(lista_mapeada));


	buffer_serializar_int(buffer, cant_lista_afinidades);

	for (int i=0; i<cant_lista_afinidades; i++)
			{char* afinidad =  list_get(datos->lista_afinidades, i);
			buffer_serializar_string(buffer, afinidad);	}

	buffer_serializar_int(buffer, cant_lista_precios);

	for (int i=0; i<cant_lista_precios; i++)
		{
		t_precio* datos_precio =  list_get(datos->lista_precios, i);
		buffer_serializar_string(buffer, datos_precio->nombre_plato);
		buffer_serializar_int(buffer, datos_precio->precio);
		}

	buffer_serializar_int(buffer, datos->posicion->x);
	buffer_serializar_int(buffer, datos->posicion->y);
	buffer_serializar_int(buffer, datos->cantidad_cocineros);
	buffer_serializar_int(buffer, datos->cantidad_hornos);
	buffer_serializar_int(buffer, datos->cantidad_pedidos);

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

static t_modulo deserializar_modulo(t_buffer* buffer)
{
	t_modulo* modulo = buffer_deserializar(buffer, sizeof(t_modulo));
	return *modulo;
}

/*static void* deserializar_posicion(t_buffer* buffer)
{
	int x = buffer_deserializar_int(buffer);
	int y = buffer_deserializar_int(buffer);

	return posicion_crear(x, y);
}*/

static void* deserializar_datos_cliente(t_buffer* buffer)
{
	char* id_cliente = buffer_deserializar_string(buffer);
	uint32_t x = buffer_deserializar_int(buffer);
	uint32_t y = buffer_deserializar_int(buffer);

	return crear_datos_cliente(id_cliente, posicion_crear(x, y));
}

static void* deserializar_datos_pedido(t_buffer* buffer)
{
	int id_pedido = buffer_deserializar_int(buffer);
	char* comida = buffer_deserializar_string(buffer);

	return crear_datos_pedido(id_pedido, comida);
}

static void* deserializar_seleccion_restaurante(t_buffer* buffer)
{
	char* id_cliente = buffer_deserializar_string(buffer);
	char* restaurante = buffer_deserializar_string(buffer);

	return crear_datos_seleccion_restaurante(id_cliente, restaurante);
}

static void* deserializar_guardar_plato(t_buffer* buffer)
{
	int id_pedido = buffer_deserializar_int(buffer);
	int cantidad = buffer_deserializar_int(buffer);
	char* comida = buffer_deserializar_string(buffer);
	char* restaurante = buffer_deserializar_string(buffer);

	return crear_datos_guardar_plato(id_pedido, cantidad, comida, restaurante);
}

static void* deserializar_agregar_plato(t_buffer* buffer)
{
	int id_pedido = buffer_deserializar_int(buffer);
	char* plato = buffer_deserializar_string(buffer);

	return crear_datos_agregar_plato(id_pedido, plato);
}


static void* deserializar_obtener_receta(t_buffer* buffer)
{
	int cantidad_pasos = buffer_deserializar_int(buffer);
	t_list* pasos = list_create();

	for (int i=0; i<cantidad_pasos; i++)
		{
		char* operacion = buffer_deserializar_string(buffer);
		int tiempo = buffer_deserializar_int(buffer);

		list_add(pasos, crear_paso(operacion, tiempo));
		}

	t_obtener_receta* datos = malloc(sizeof(t_obtener_receta));
	datos->pasos = pasos;
	return datos;
}

static void* deserializar_obtener_restaurante(t_buffer* buffer)
{
	int cantidad_afinidades = buffer_deserializar_int(buffer);
	t_list* afinidades = list_create();

	for (int i=0; i<cantidad_afinidades; i++)
		{
		char* afinidad = buffer_deserializar_string(buffer);
		list_add(afinidades, afinidad);	}

	int cantidad_precios = buffer_deserializar_int(buffer);

	t_list* lista_precios = list_create();
	for (int i=0; i<cantidad_precios; i++)
		{
		char* nombre = buffer_deserializar_string(buffer);
		int precio = buffer_deserializar_int(buffer);
		list_add(lista_precios, crear_precio(nombre, precio));	}

	int posx = buffer_deserializar_int(buffer);
	int posy = buffer_deserializar_int(buffer);
	t_posicion* posicion = posicion_crear(posx, posy);

	int cant_coci = buffer_deserializar_int(buffer);
	int cant_hornos = buffer_deserializar_int(buffer);
	int cant_pedidos = buffer_deserializar_int(buffer);

	return crear_datos_obtener_restaurante(afinidades, lista_precios, posicion, cant_coci, cant_hornos, cant_pedidos);
}

static void* deserializar_estado_pedido(t_buffer* buffer)
{
	t_estado_pedido* estado_pedido = buffer_deserializar(buffer, sizeof(t_estado_pedido));

	int cantidad_platos = buffer_deserializar_int(buffer);
	t_list* platos = list_create();

	for (int i=0; i<cantidad_platos; i++)
		{
		int lista = buffer_deserializar_int(buffer);
		int total = buffer_deserializar_int(buffer);
		char* comida = buffer_deserializar_string(buffer);

		list_add(platos, crear_datos_estado_comida(comida, total, lista));
		}

	return crear_datos_estado_pedido(*estado_pedido, platos);
}


static void* deserializar_consultar_pedido(t_buffer* buffer)
{
	char* restaurante = buffer_deserializar_string(buffer);
	t_estado_pedido* estado_pedido = buffer_deserializar(buffer, sizeof(t_estado_pedido));

	int cantidad_platos = buffer_deserializar_int(buffer);
	t_list* platos = list_create();

	for (int i=0; i<cantidad_platos; i++)
		{
		int lista = buffer_deserializar_int(buffer);
		int total = buffer_deserializar_int(buffer);
		char* comida = buffer_deserializar_string(buffer);

		list_add(platos, crear_datos_estado_comida(comida, total, lista));
		}

	return crear_datos_consultar_pedido(restaurante, *estado_pedido, platos);
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

void destruir_datos_cliente(void* datos_void)
{
	t_datos_cliente* datos = datos_void;
	free(datos->id_cliente);
	free(datos->posicion);
	free(datos);
}

void destruir_datos_pedido(void* datos_void)
{
	t_datos_pedido* datos = datos_void;
	free(datos->restaurante);
	free(datos);
}

void destruir_datos_seleccion_restaurante(void* datos_void)
{
	t_datos_seleccion_restaurante* datos = datos_void;
	free(datos->restaurante);
	free(datos->id_cliente);
	free(datos);
}

void destruir_guardar_plato(void* datos_void)
{
	t_guardar_plato* datos = datos_void;
	free(datos->comida);
	free(datos->restaurante);
	free(datos);
}

void destruir_agregar_plato(void* datos_void)
{
	t_agregar_plato* datos = datos_void;
	free(datos->plato);
	free(datos);
}

void destruir_consultar_pedido(void* datos_void)
{
	t_consultar_pedido* datos = datos_void;
	destruir_lista_string(datos->platos);
	free(datos->restaurante);
	free(datos);
}

void destruir_handshake_restaurante_app(void* datos_void)
{
	t_handshake_resto_app* datos = datos_void;
	free(datos->restaurante);
	free(datos);
}


void destruir_estado_pedido(void* datos_void)
{
	t_datos_estado_pedido* datos_estado_pedido = datos_void;

	void destruir_platos (void* platos) { free(((t_datos_estado_comida*) platos)->comida);free(platos);}

	list_destroy_and_destroy_elements(datos_estado_pedido->platos, &destruir_platos);
	free(datos_void);
}

static void sin_free() { }

// ===== Diccionario de Serializaciones =====
void diccionario_serializaciones_inicializar()
{
	diccionario_serializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_serializaciones, HANDSHAKE_CLIENTE, &serializar_sin_datos);
	dictionary_int_put(diccionario_serializaciones, HANDSHAKE_CLIENTE_RESPUESTA, &serializar_modulo);
	dictionary_int_put(diccionario_serializaciones, CONEXION_CLIENTE, &serializar_datos_cliente);
	dictionary_int_put(diccionario_serializaciones, CONEXION_CLIENTE_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, HANDSHAKE_RESTO_APP, &serializar_handshake_restaurante_app);
	dictionary_int_put(diccionario_serializaciones, HANDSHAKE_RESTO_APP_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, HANDSHAKE_RESTO_SIND, &serializar_sin_datos);
	dictionary_int_put(diccionario_serializaciones, HANDSHAKE_RESTO_SIND_RESPUESTA, &serializar_bool);

	dictionary_int_put(diccionario_serializaciones, CONSULTAR_RESTAURANTES, &serializar_sin_datos);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_RESTAURANTES_RESPUESTA, &serializar_lista_string);
	dictionary_int_put(diccionario_serializaciones, SELECCIONAR_RESTAURANTE, &serializar_seleccion_restaurante);
	dictionary_int_put(diccionario_serializaciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_PLATOS, &serializar_string);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_PLATOS_RESPUESTA, &serializar_lista_string);
	dictionary_int_put(diccionario_serializaciones, CONFIRMAR_PEDIDO, &serializar_datos_pedido);
	dictionary_int_put(diccionario_serializaciones, CONFIRMAR_PEDIDO_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, GUARDAR_PEDIDO, &serializar_datos_pedido);
	dictionary_int_put(diccionario_serializaciones, GUARDAR_PEDIDO_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, CREAR_PEDIDO, &serializar_string);
	dictionary_int_put(diccionario_serializaciones, CREAR_PEDIDO_RESPUESTA, &serializar_int);
	dictionary_int_put(diccionario_serializaciones, AGREGAR_PLATO, &serializar_agregar_plato);
	dictionary_int_put(diccionario_serializaciones, AGREGAR_PLATO_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, GUARDAR_PLATO, &serializar_guardar_plato);
	dictionary_int_put(diccionario_serializaciones, GUARDAR_PLATO_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, PLATO_LISTO, &serializar_guardar_plato);
	dictionary_int_put(diccionario_serializaciones, PLATO_LISTO_RESPUESTA, &serializar_bool);

	dictionary_int_put(diccionario_serializaciones, OBTENER_RECETA, &serializar_string);
	dictionary_int_put(diccionario_serializaciones, OBTENER_RECETA_RESPUESTA, &serializar_obtener_receta);
	dictionary_int_put(diccionario_serializaciones, OBTENER_RESTAURANTE, &serializar_string);
	dictionary_int_put(diccionario_serializaciones, OBTENER_RESTAURANTE_RESPUESTA, &serializar_obtener_restaurante);
	dictionary_int_put(diccionario_serializaciones, FINALIZAR_PEDIDO, &serializar_datos_pedido);
	dictionary_int_put(diccionario_serializaciones, FINALIZAR_PEDIDO_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, TERMINAR_PEDIDO, &serializar_datos_pedido);
	dictionary_int_put(diccionario_serializaciones, TERMINAR_PEDIDO_RESPUESTA, &serializar_bool);
	dictionary_int_put(diccionario_serializaciones, OBTENER_PEDIDO, &serializar_datos_pedido);
	dictionary_int_put(diccionario_serializaciones, OBTENER_PEDIDO_RESPUESTA, &serializar_estado_pedido);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_PEDIDO, &serializar_int);
	dictionary_int_put(diccionario_serializaciones, CONSULTAR_PEDIDO_RESPUESTA, &serializar_consultar_pedido);
}

void diccionario_serializaciones_destruir()
{
	dictionary_int_destroy(diccionario_serializaciones);
}

// ===== Diccionario de Deserializaciones =====
void diccionario_deserializaciones_inicializar()
{
	diccionario_deserializaciones = dictionary_int_create();

	dictionary_int_put(diccionario_deserializaciones, HANDSHAKE_CLIENTE, &deserializar_sin_datos);
	dictionary_int_put(diccionario_deserializaciones, HANDSHAKE_CLIENTE_RESPUESTA, &deserializar_modulo);
	dictionary_int_put(diccionario_deserializaciones, CONEXION_CLIENTE, &deserializar_datos_cliente);
	dictionary_int_put(diccionario_deserializaciones, CONEXION_CLIENTE_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, HANDSHAKE_RESTO_APP, &deserializar_handshake_restaurante_app);
	dictionary_int_put(diccionario_deserializaciones, HANDSHAKE_RESTO_APP_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, HANDSHAKE_RESTO_SIND, &deserializar_sin_datos);
	dictionary_int_put(diccionario_deserializaciones, HANDSHAKE_RESTO_SIND_RESPUESTA, &deserializar_bool);

	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_RESTAURANTES, &deserializar_sin_datos);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_RESTAURANTES_RESPUESTA, &deserializar_lista_string);
	dictionary_int_put(diccionario_deserializaciones, SELECCIONAR_RESTAURANTE, &deserializar_seleccion_restaurante);
	dictionary_int_put(diccionario_deserializaciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_PLATOS, &deserializar_string);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_PLATOS_RESPUESTA, &deserializar_lista_string);
	dictionary_int_put(diccionario_deserializaciones, CREAR_PEDIDO, &deserializar_string);
	dictionary_int_put(diccionario_deserializaciones, CREAR_PEDIDO_RESPUESTA, &deserializar_int);
	dictionary_int_put(diccionario_deserializaciones, CONFIRMAR_PEDIDO, &deserializar_datos_pedido);
	dictionary_int_put(diccionario_deserializaciones, CONFIRMAR_PEDIDO_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, GUARDAR_PEDIDO, &deserializar_datos_pedido);
	dictionary_int_put(diccionario_deserializaciones, GUARDAR_PEDIDO_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, AGREGAR_PLATO, &deserializar_agregar_plato);
	dictionary_int_put(diccionario_deserializaciones, AGREGAR_PLATO_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, GUARDAR_PLATO, &deserializar_guardar_plato);
	dictionary_int_put(diccionario_deserializaciones, GUARDAR_PLATO_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, PLATO_LISTO, &deserializar_guardar_plato);
	dictionary_int_put(diccionario_deserializaciones, PLATO_LISTO_RESPUESTA, &deserializar_bool);

	dictionary_int_put(diccionario_deserializaciones, OBTENER_RECETA, &deserializar_string);
	dictionary_int_put(diccionario_deserializaciones, OBTENER_RECETA_RESPUESTA, &deserializar_obtener_receta);
	dictionary_int_put(diccionario_deserializaciones, OBTENER_RESTAURANTE, &deserializar_string);
	dictionary_int_put(diccionario_deserializaciones, OBTENER_RESTAURANTE_RESPUESTA, &deserializar_obtener_restaurante);
	dictionary_int_put(diccionario_deserializaciones, FINALIZAR_PEDIDO, &deserializar_datos_pedido);
	dictionary_int_put(diccionario_deserializaciones, FINALIZAR_PEDIDO_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, TERMINAR_PEDIDO, &deserializar_datos_pedido);
	dictionary_int_put(diccionario_deserializaciones, TERMINAR_PEDIDO_RESPUESTA, &deserializar_bool);
	dictionary_int_put(diccionario_deserializaciones, OBTENER_PEDIDO, &deserializar_datos_pedido);
	dictionary_int_put(diccionario_deserializaciones, OBTENER_PEDIDO_RESPUESTA, &deserializar_estado_pedido);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_PEDIDO, &deserializar_int);
	dictionary_int_put(diccionario_deserializaciones, CONSULTAR_PEDIDO_RESPUESTA, &deserializar_consultar_pedido);
}

// ===== Diccionario de Destrucciones =====
void diccionario_destrucciones_inicializar()
{
	diccionario_destrucciones = dictionary_int_create();

	dictionary_int_put(diccionario_destrucciones, HANDSHAKE_CLIENTE, &sin_free);
	dictionary_int_put(diccionario_destrucciones, HANDSHAKE_CLIENTE_RESPUESTA, &sin_free);
	dictionary_int_put(diccionario_destrucciones, CONEXION_CLIENTE, &destruir_datos_cliente);
	dictionary_int_put(diccionario_destrucciones, CONEXION_CLIENTE_RESPUESTA, &free);
	dictionary_int_put(diccionario_destrucciones, HANDSHAKE_RESTO_APP, &destruir_handshake_restaurante_app);
	dictionary_int_put(diccionario_destrucciones, HANDSHAKE_RESTO_APP_RESPUESTA, &free);
	dictionary_int_put(diccionario_destrucciones, HANDSHAKE_RESTO_SIND, &sin_free);
	dictionary_int_put(diccionario_destrucciones, HANDSHAKE_RESTO_SIND_RESPUESTA, &free);

	dictionary_int_put(diccionario_destrucciones, CONSULTAR_RESTAURANTES, &sin_free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_RESTAURANTES_RESPUESTA, &destruir_lista_string);
	dictionary_int_put(diccionario_destrucciones, SELECCIONAR_RESTAURANTE, &destruir_datos_seleccion_restaurante);
	dictionary_int_put(diccionario_destrucciones, SELECCIONAR_RESTAURANTE_RESPUESTA, &free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_PLATOS, &free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_PLATOS_RESPUESTA, &destruir_lista_string);
	dictionary_int_put(diccionario_destrucciones, CREAR_PEDIDO, &free);
	dictionary_int_put(diccionario_destrucciones, CREAR_PEDIDO_RESPUESTA, &sin_free);
	dictionary_int_put(diccionario_destrucciones, CONFIRMAR_PEDIDO, &destruir_datos_pedido);
	dictionary_int_put(diccionario_destrucciones, CONFIRMAR_PEDIDO_RESPUESTA, &sin_free);
	dictionary_int_put(diccionario_destrucciones, GUARDAR_PEDIDO, &destruir_datos_pedido);
	dictionary_int_put(diccionario_destrucciones, GUARDAR_PEDIDO_RESPUESTA, &sin_free);
	dictionary_int_put(diccionario_destrucciones, AGREGAR_PLATO, &destruir_agregar_plato);
	dictionary_int_put(diccionario_destrucciones, AGREGAR_PLATO_RESPUESTA, &sin_free);
	dictionary_int_put(diccionario_destrucciones, GUARDAR_PLATO, &destruir_guardar_plato);
	dictionary_int_put(diccionario_destrucciones, GUARDAR_PLATO_RESPUESTA, &sin_free);
	dictionary_int_put(diccionario_destrucciones, PLATO_LISTO, &destruir_guardar_plato);
	dictionary_int_put(diccionario_destrucciones, PLATO_LISTO_RESPUESTA, &sin_free);

	dictionary_int_put(diccionario_destrucciones, OBTENER_RECETA, &free);
	dictionary_int_put(diccionario_destrucciones, OBTENER_RECETA_RESPUESTA, &sin_free);//ver free
	dictionary_int_put(diccionario_destrucciones, OBTENER_RESTAURANTE, &free);
	dictionary_int_put(diccionario_destrucciones, OBTENER_RESTAURANTE_RESPUESTA, &sin_free);//ver free
	dictionary_int_put(diccionario_destrucciones, FINALIZAR_PEDIDO, &destruir_datos_pedido);
	dictionary_int_put(diccionario_destrucciones, FINALIZAR_PEDIDO_RESPUESTA, &sin_free);
	dictionary_int_put(diccionario_destrucciones, TERMINAR_PEDIDO, &destruir_datos_pedido);
	dictionary_int_put(diccionario_destrucciones, TERMINAR_PEDIDO_RESPUESTA, &sin_free);
	dictionary_int_put(diccionario_destrucciones, OBTENER_PEDIDO, &destruir_datos_pedido);
	dictionary_int_put(diccionario_destrucciones, OBTENER_PEDIDO_RESPUESTA, &sin_free); //ver free
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_PEDIDO, &sin_free);
	dictionary_int_put(diccionario_destrucciones, CONSULTAR_PEDIDO_RESPUESTA, &sin_free);//ver free
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
	dictionary_int_put(diccionario_destruccion_respuestas, OBTENER_PEDIDO_RESPUESTA, &destruir_estado_pedido);
	dictionary_int_put(diccionario_destruccion_respuestas, CONSULTAR_PEDIDO_RESPUESTA, &destruir_consultar_pedido);
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

