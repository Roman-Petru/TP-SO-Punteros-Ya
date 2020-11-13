#include "algoritmos_reemplazo.h"

t_list* paginas_en_memoria;
pthread_mutex_t mutex_algoritmos;

static int algoritmo_LRU(t_pagina* pagina)
{
	pthread_mutex_lock(&mutex_algoritmos);
	t_pagina* pagina_victima = list_remove(paginas_en_memoria, 0);
	pthread_mutex_unlock(&mutex_algoritmos);
	int index = pagina_victima->marco_principal;

	log_info(logger, "Se selecciono la pagina que estaba en el marco %d como victima para reemplazar, elegido por el algoritmo LRU", pagina_victima->marco_principal);

	if(pagina_victima->modificado)
		escribir_marco_virtual(pagina_victima);

	pagina_victima->validacion_principal = false;
	pagina_victima->modificado = false;

	return index;
}

static t_pagina* buscar_victima_clock()
{
	if (puntero_a_paginas >= list_size(paginas_en_memoria))
		puntero_a_paginas = 0;

	bool encontro = false;
	int primera_vuelta = true;
	int indice_pagina_victima;

	for(;!encontro;puntero_a_paginas++)
		{
		t_pagina* pivot = list_get(paginas_en_memoria, puntero_a_paginas);
			if(primera_vuelta){
				encontro = !pivot->uso && !pivot->modificado;
			} else {
				encontro = !pivot->uso && pivot->modificado;
				if (!encontro)
					pivot->uso = false;}

		//log_info(logger, "Estamos en la vuelta %d y el puntero esta en %d", primera_vuelta, puntero_a_paginas);

		if (encontro)
			indice_pagina_victima = puntero_a_paginas;
		if (puntero_a_paginas + 1 == list_size(paginas_en_memoria))
			{puntero_a_paginas = -1; primera_vuelta= !primera_vuelta;}
		}

	return list_remove(paginas_en_memoria, indice_pagina_victima);

}

static int algoritmo_clock_mejorado(t_pagina* pagina)
{
	pthread_mutex_lock(&mutex_algoritmos);
	t_pagina* pagina_victima = buscar_victima_clock();
	pthread_mutex_unlock(&mutex_algoritmos);

	int index = pagina_victima->marco_principal;

	log_info(logger, "Se selecciono la pagina que estaba en el marco %d como victima para reemplazar, elegido por el algoritmo Clock-Mejorado", pagina_victima->marco_principal);

	if(pagina_victima->modificado)
		escribir_marco_virtual(pagina_victima);

	pagina_victima->validacion_principal = false;
	pagina_victima->modificado = false;

	return index;
}

int algoritmo_reemplazo(t_pagina* pagina)
{
	char* algoritmo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");

	if(strcmp(algoritmo, "LRU")==0)
		return algoritmo_LRU(pagina);

	if(strcmp(algoritmo, "CLOCK_MEJ")==0)
		return algoritmo_clock_mejorado(pagina);

	return -1;
}

void sacar_de_paginas_en_memoria(t_pagina* pagina_buscada)
{

	pthread_mutex_lock(&mutex_algoritmos);
	for (int i=0; i<list_size(paginas_en_memoria); i++)
		{t_pagina* pivot = list_get(paginas_en_memoria, i);
		if (pivot->id == pagina_buscada->id)
			{list_remove(paginas_en_memoria, i);
			if (puntero_a_paginas > i)
				puntero_a_paginas--;
			}
		}
	pthread_mutex_unlock(&mutex_algoritmos);

	//bool misma_pagina(t_pagina* pagina) { return pagina->id == pagina_buscada->id; }
	//list_remove_by_condition(paginas_en_memoria, (void*) &misma_pagina);
}

void mover_al_final(t_pagina* pagina_buscada)
{
	bool misma_pagina(t_pagina* pagina) { return pagina->id == pagina_buscada->id; }

	pthread_mutex_lock(&mutex_algoritmos);
	list_remove_by_condition(paginas_en_memoria, (void*) &misma_pagina);
	list_add(paginas_en_memoria, pagina_buscada);
	pthread_mutex_unlock(&mutex_algoritmos);
}
