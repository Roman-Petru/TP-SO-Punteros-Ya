#include "algoritmos_reemplazo.h"

t_list* paginas_en_memoria;
pthread_mutex_t mutex_algoritmos;

static int algoritmo_LRU(t_pagina* pagina)
{
	t_pagina* pagina_victima = list_remove(paginas_en_memoria, 0);
	int index = pagina_victima->marco_principal;

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

	return -1;
}

void sacar_de_paginas_en_memoria(t_pagina* pagina_buscada)
{
	bool misma_pagina(t_pagina* pagina) { return pagina->id == pagina_buscada->id; }

	pthread_mutex_lock(&mutex_algoritmos);
	list_remove_by_condition(paginas_en_memoria, (void*) &misma_pagina);
	pthread_mutex_unlock(&mutex_algoritmos);
}

void mover_al_final(t_pagina* pagina_buscada)
{
	bool misma_pagina(t_pagina* pagina) { return pagina->id == pagina_buscada->id; }

	pthread_mutex_lock(&mutex_algoritmos);
	list_remove_by_condition(paginas_en_memoria, (void*) &misma_pagina);
	list_add(paginas_en_memoria, pagina_buscada);
	pthread_mutex_unlock(&mutex_algoritmos);
}
