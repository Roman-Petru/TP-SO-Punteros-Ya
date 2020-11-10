#include "gestor_tablas.h"


pthread_mutex_t mutex_tablas;


void inicializar_gestor_tablas()
{
	tabla_restaurantes = list_create();
	pthread_mutex_init(&mutex_tablas, NULL);
}


bool tabla_restaurante_existe(char* restaurante)
{
	bool es_mismo_restaurante(void* resto) { return strcmp(restaurante, resto)==0; }
	return list_any_satisfy(tabla_restaurantes, &es_mismo_restaurante);
}

bool segmento_existe(char* restaurante, int id_pedido)
{
	t_restaurante* restaurante_buscado = obtener_tabla_restaurante(restaurante);

	bool es_mismo_segmento(void* id) { return strcmp(id_pedido, id)==0; }  //NO ESTOY SEGURO COMO HACER ESTO
	return list_any_satisfy(restaurante_buscado->tabla_segmentos, &es_mismo_segmento);
}

void tabla_restaurante_crear(char* nombre_restaurante)
{
	t_restaurante* restaurante = malloc(sizeof(t_restaurante));
	restaurante->restaurante = nombre_restaurante;
	restaurante->tabla_segmentos = list_create();
	pthread_mutex_init(&(restaurante->mutex_tabla_segmentos), NULL);

	pthread_mutex_lock(&mutex_tablas);
	list_add(tabla_restaurantes, restaurante);
	pthread_mutex_unlock(&mutex_tablas);
}


static int restaurante_index(char* nombre_restaurante)
{
	int index;
	bool encontro = false;
	bool es_mismo_restaurante(void* restaurante) { return strcmp(((t_restaurante*) restaurante)->restaurante, nombre_restaurante) == 0; }

	for(index = 0; !encontro && list_get(tabla_restaurantes, index) != NULL; index++)
		encontro = es_mismo_restaurante(list_get(tabla_restaurantes, index));

	return encontro ? index-1 : -1;
}

static int segmento_index(t_restaurante* restaurante_buscado, int id_pedido)
{
	int index;
	bool encontro = false;
	bool es_mismo_segmento(void* id) { return strcmp(((t_segmento*) id)->id_pedido, id_pedido) == 0; }

	for(index = 0; !encontro && list_get(restaurante_buscado->tabla_segmentos, index) != NULL; index++)
		encontro = es_mismo_segmento(list_get(restaurante_buscado->tabla_segmentos, index));

	return encontro ? index-1 : -1;
}



t_restaurante* obtener_tabla_restaurante(char* restaurante)
{
	pthread_mutex_lock(&mutex_tablas);
	int i = restaurante_index(restaurante);
	t_restaurante* restaurante_buscado = list_get(tabla_restaurantes, i);
	pthread_mutex_unlock(&mutex_tablas);

	return restaurante_buscado;
}


t_segmento* obtener_segmento(char* restaurante, int id_pedido)
{
	t_restaurante* restaurante_buscado = obtener_tabla_restaurante(restaurante);

	pthread_mutex_lock(&(restaurante_buscado->mutex_tabla_segmentos));
	int i = segmento_index(restaurante_buscado, id_pedido);
	t_segmento segmento = list_get(restaurante_buscado->tabla_segmentos, i);
	pthread_mutex_unlock(&(restaurante_buscado->mutex_tabla_segmentos));

	return segmento;
}

bool tabla_restaurante_agregar_tabla_segmentos(t_datos_pedido* datos)
{
	t_segmento* segmento = malloc(sizeof(t_segmento));
	if (segmento == NULL)
		return false;

	segmento->id_pedido = datos->id_pedido;
	segmento->estado = PENDIENTE;
	segmento->tabla_paginas = list_create();

	t_restaurante* restaurante = obtener_tabla_restaurante(datos->restaurante);

	pthread_mutex_lock(&(restaurante->mutex_tabla_segmentos));
	list_add(restaurante->tabla_segmentos, segmento);
	pthread_mutex_unlock(&(restaurante->mutex_tabla_segmentos));

	return true;
}






