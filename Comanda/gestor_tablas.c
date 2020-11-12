#include "gestor_tablas.h"
#include "comanda.h"

pthread_mutex_t mutex_tablas;

int id_para_algoritmos;

void inicializar_gestor_tablas()
{
	tabla_restaurantes = list_create();
	pthread_mutex_init(&mutex_tablas, NULL);

	id_para_algoritmos = 0;
}

bool tabla_segmento_restaurante_existe(char* restaurante)
{
	/*bool es_mismo_restaurante(void* resto) { return strcmp(restaurante, resto)==0; }
	return list_any_satisfy(tabla_restaurantes, &es_mismo_restaurante);*/

	for(int i=0; i < list_size(tabla_restaurantes);i++)
		{
			t_restaurante* resto_pivot = list_get(tabla_restaurantes, i);
			if (strcmp(restaurante, resto_pivot->restaurante)==0)
				return true;
		}
	return false;
}

bool segmento_existe(char* restaurante, int id_pedido)
{
	t_restaurante* restaurante_buscado = obtener_tabla_restaurante(restaurante);

	bool es_mismo_segmento(void* segmento) { return ((t_segmento*) segmento)->id_pedido == id_pedido; }
	return list_any_satisfy(restaurante_buscado->tabla_segmentos, &es_mismo_segmento);
}

bool pagina_existe(t_segmento* segmento, char* comida)
{
	bool es_mismo_plato(void* pagina) { return strcmp(((t_pagina*)pagina)->comida, comida)==0; }
	return list_any_satisfy(segmento->tabla_paginas, &es_mismo_plato);
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
	bool es_mismo_segmento(void* id) { return ((t_segmento*) id)->id_pedido == id_pedido; }

	for(index = 0; !encontro && list_get(restaurante_buscado->tabla_segmentos, index) != NULL; index++)
		encontro = es_mismo_segmento(list_get(restaurante_buscado->tabla_segmentos, index));

	return encontro ? index-1 : -1;
}

static int pagina_index(t_segmento* segmento, char* comida)
{
	int index;
	bool encontro = false;
	bool es_mismo_plato(void* pagina) { return strcmp(((t_pagina*)pagina)->comida, comida)==0; }

	for(index = 0; !encontro && list_get(segmento->tabla_paginas, index) != NULL; index++)
		encontro = es_mismo_plato(list_get(segmento->tabla_paginas, index));

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
	t_segmento* segmento = list_get(restaurante_buscado->tabla_segmentos, i);
	pthread_mutex_unlock(&(restaurante_buscado->mutex_tabla_segmentos));

	return segmento;
}


t_pagina* obtener_pagina(t_segmento* segmento, char* comida)
{
	pthread_mutex_lock(&(segmento->mutex_tabla_paginas));
	int i = pagina_index(segmento, comida);
	t_pagina* pagina= list_get(segmento->tabla_paginas, i);
	pthread_mutex_unlock(&(segmento->mutex_tabla_paginas));

	return pagina;
}

void tabla_restaurante_crear(char* nombre_restaurante)
{
	t_restaurante* restaurante = malloc(sizeof(t_restaurante));

	size_t tam = strlen(nombre_restaurante)+1;
	restaurante->restaurante = malloc(tam);
	memcpy(restaurante->restaurante, nombre_restaurante, tam);

	//restaurante->restaurante = malloc(strlen(nombre_restaurante)+1);
	//strcpy(restaurante->restaurante, nombre_restaurante);

	restaurante->tabla_segmentos = list_create();
	pthread_mutex_init(&(restaurante->mutex_tabla_segmentos), NULL);

	pthread_mutex_lock(&mutex_tablas);
	list_add(tabla_restaurantes, restaurante);
	pthread_mutex_unlock(&mutex_tablas);

	log_info(logger, "Se creo tabla de segmenstos para nuevo restaurante %s", nombre_restaurante);
}

bool tabla_restaurante_agregar_segmento(t_datos_pedido* datos)
{
	t_segmento* segmento = malloc(sizeof(t_segmento));
	if (segmento == NULL)
		return false;

	segmento->id_pedido = datos->id_pedido;
	segmento->estado = PENDIENTE;
	segmento->tabla_paginas = list_create();
	pthread_mutex_init(&(segmento->mutex_tabla_paginas), NULL);

	t_restaurante* restaurante = obtener_tabla_restaurante(datos->restaurante);

	pthread_mutex_lock(&(restaurante->mutex_tabla_segmentos));
	list_add(restaurante->tabla_segmentos, segmento);
	pthread_mutex_unlock(&(restaurante->mutex_tabla_segmentos));

	log_info(logger, "Se creo tabla de paginas para nuevo pedido %d", segmento->id_pedido);

	return true;
}

bool asignar_nueva_pagina(t_segmento* segmento, char* comida)
{
	t_pagina* pagina = malloc(sizeof(t_pagina));
	if (pagina == NULL)
		return false;

	pagina->id = id_para_algoritmos;
	id_para_algoritmos++;

	size_t tam = strlen(comida)+1;
	pagina->comida = malloc(tam);
	memcpy(pagina->comida, comida, tam);

	pagina->validacion_principal = 0;
	pagina->validacion_virtual= 0;
	pagina->uso = 0;
	pagina->modificado = 0;
	pagina->comida_esta_lista = false;

	pthread_mutex_lock(&(segmento->mutex_tabla_paginas));
	list_add(segmento->tabla_paginas, pagina);
	pthread_mutex_unlock(&(segmento->mutex_tabla_paginas));

	log_info(logger, "Se creo una nueva pagina con el plato %s para el pedido %d", pagina->comida, segmento->id_pedido);

	return true;
}

void tabla_restaurante_eliminar_segmento(t_segmento* segmento)
{
	pthread_mutex_destroy(&(segmento->mutex_tabla_paginas));
	free(segmento);
}

void eliminar_paginas(t_segmento* segmento)
{
	void destruir_pagina(void* pagina_void)
	{
		t_pagina* pagina = pagina_void;
		if(pagina->validacion_principal)
			vaciar_pagina_memoria_principal(pagina);
		vaciar_pagina_memoria_virtual(pagina);
		free(pagina->comida);
		free(pagina);
	}

	list_destroy_and_destroy_elements(segmento->tabla_paginas, &destruir_pagina);
}


