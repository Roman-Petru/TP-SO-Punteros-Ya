#include "memoria_virtual.h"
#include "comanda.h"

t_list* tabla_marcos_virtual;
int cantidad_frames_virtual;
t_bitarray* mapa_bits_virtual;
pthread_mutex_t mutex;
pthread_mutex_t mutex_mapa_bit;

t_marco* crear_marco()
{
	t_marco* marco = malloc(sizeof(t_marco));
	return marco;
}

int primer_marco_virtual_libre()
{
	int index;
	bool encontro = false;

	pthread_mutex_lock(&mutex_mapa_bit);
	for(index=0;!encontro && index<cantidad_frames_virtual;index++)
		encontro = bitarray_test_bit(mapa_bits_virtual, index);
	if (encontro)
		bitarray_clean_bit(mapa_bits_virtual, index);
	pthread_mutex_unlock(&mutex_mapa_bit);

	return encontro ? index-1 : -1;
}

t_marco* obtener_marco_virtual(int index)
{
	t_marco* marco;
	pthread_mutex_lock(&mutex);
	marco = list_get(tabla_marcos_virtual, index);
	pthread_mutex_unlock(&mutex);
	return marco;
}

void cargar_marco_virtual(int index, t_pagina* pagina)
{
	t_marco* marco = obtener_marco_virtual(index);
	marco->nombre_plato = pagina->comida;
	marco->cantidad_total = 0;
	marco->cantidad_lista = 0;

	pagina->marco_virtual = index;
	pagina->validacion_virtual = true;
}

void escribir_marco_virtual(t_pagina* pagina)
{
	t_marco* marco_virtual = obtener_marco_virtual(pagina->marco_virtual);
	t_marco* marco_principal = obtener_marco_principal(pagina->marco_principal);
	marco_virtual->nombre_plato = marco_principal->nombre_plato;
	marco_virtual->cantidad_total = marco_principal->cantidad_total;
	marco_virtual->cantidad_lista = marco_principal->cantidad_lista;
}


void inicializar_memoria_virtual()
{
	cantidad_frames_virtual = (config_get_int_value(config, "TAMANIO_SWAP") / 32 );
	tabla_marcos_virtual = list_create();
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_mapa_bit, NULL);

	void* mentira = malloc(cantidad_frames_virtual / 8);
	mapa_bits_virtual = bitarray_create_with_mode(mentira, cantidad_frames_virtual / 8, MSB_FIRST);

	for (int i=0; i < bitarray_get_max_bit(mapa_bits_virtual); i++)
	{
		bitarray_set_bit(mapa_bits_virtual, i);
		list_add(tabla_marcos_virtual, crear_marco());
	}
}

int algoritmo_reemplazo(t_pagina* pagina)
{
	int index = primer_marco_principal_libre();

	if(index == -1)
		index = algoritmo_reemplazo(pagina);

	return index;
}

bool cargar_desde_swap_si_es_necesario(t_pagina* pagina)
{
	if(pagina->validacion_principal)
		return true;

	if(!pagina->validacion_virtual)
	{
		int index = primer_marco_libre();
		if(index == -1)
			return false; // LOGGEAR NO ESPACIO EN MEMORIA

		cargar_marco(index, pagina);
	}

	int index = primer_marco_principal_libre();
	if(index == -1)
		index = algoritmo_reemplazo(pagina);

	mover_al_final(pagina);
	pagina->validacion_principal = true;
	pagina->marco_principal = index;

	return true;
}

void vaciar_segmento_memoria_virtual(t_segmento* segmento)
{
	segmento->tabla_paginas;
}


void vaciar_pagina_memoria_virtual(t_pagina* pagina)
{
	bitarray_set_bit(mapa_bits_virtual, pagina->marco_virtual);
}


