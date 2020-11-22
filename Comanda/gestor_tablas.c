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
/*
bool pagina_existe(t_segmento* segmento, char* comida)
{
	bool es_mismo_plato(void* pagina) { return strcmp(((t_pagina*)pagina)->comida, comida)==0; }
	return list_any_satisfy(segmento->tabla_paginas, &es_mismo_plato);
}*/

int obtener_pagina_comida(t_segmento* segmento, char* comida)
{
	for (int i=0; i< list_size(segmento->tabla_paginas);i++)
	{
		t_pagina* pagina = list_get(segmento->tabla_paginas, i);
		cargar_desde_swap_si_es_necesario(pagina);
		t_marco* marco_principal = obtener_marco_principal(pagina->marco_principal);
		pagina->uso = true;
		log_info(logger, "Se leyo el marco de memoria principal de posicion %d con nombre de plato %s, cantidad lista %d y cantidad total %d", pagina->marco_principal, marco_principal->nombre_plato, marco_principal->cantidad_lista, marco_principal->cantidad_total);

		if (strcmp(marco_principal->nombre_plato, comida)== 0)
			return i;
	}
	return -1;
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
/*
static int pagina_index(t_segmento* segmento, char* comida)
{
	int index;
	bool encontro = false;
	bool es_mismo_plato(void* pagina) { return strcmp(((t_pagina*)pagina)->comida, comida)==0; }

	for(index = 0; !encontro && list_get(segmento->tabla_paginas, index) != NULL; index++)
		encontro = es_mismo_plato(list_get(segmento->tabla_paginas, index));

	return encontro ? index-1 : -1;
}*/


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

/*
t_pagina* obtener_pagina(t_segmento* segmento, char* comida)
{
	pthread_mutex_lock(&(segmento->mutex_tabla_paginas));
	int i = pagina_index(segmento, comida);
	t_pagina* pagina= list_get(segmento->tabla_paginas, i);
	pthread_mutex_unlock(&(segmento->mutex_tabla_paginas));

	return pagina;
}
*/
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

int asignar_nueva_pagina(t_segmento* segmento)
{
	t_pagina* pagina = malloc(sizeof(t_pagina));
	if (pagina == NULL)
		return false;

	pagina->id = id_para_algoritmos;
	id_para_algoritmos++;

	pagina->validacion_principal = 0;
	pagina->validacion_virtual= 0;
	pagina->uso = 0;
	pagina->modificado = 0;
	pagina->comida_esta_lista = false;

	//pthread_mutex_lock(&(segmento->mutex_tabla_paginas));
	list_add(segmento->tabla_paginas, pagina);
	//pthread_mutex_unlock(&(segmento->mutex_tabla_paginas));

	log_info(logger, "Se creo una nueva pagina para el pedido %d",segmento->id_pedido);

	return (list_size(segmento->tabla_paginas))-1;
}

void tabla_restaurante_eliminar_segmento(char* restaurante, t_segmento* segmento)
{
	t_restaurante* resto = obtener_tabla_restaurante(restaurante);
	pthread_mutex_lock(&(resto->mutex_tabla_segmentos));
	list_remove(resto->tabla_segmentos, segmento_index(resto, segmento->id_pedido));
	pthread_mutex_unlock(&(resto->mutex_tabla_segmentos));

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
		free(pagina);
	}

	list_destroy_and_destroy_elements(segmento->tabla_paginas, &destruir_pagina);
}
/*
void eliminar_pagina_falta_memoria(t_segmento* segmento,t_pagina* pagina)
{
	pthread_mutex_lock(&(segmento->mutex_tabla_paginas));
	list_remove(segmento->tabla_paginas, pagina_index(segmento, pagina->comida));
	pthread_mutex_unlock(&(segmento->mutex_tabla_paginas));

	log_info(logger, "Se elimino la pagina con el plato %s para el pedido %d por insuficiente espacio en memoria virtual", pagina->comida, segmento->id_pedido);

	free(pagina);
}*/
