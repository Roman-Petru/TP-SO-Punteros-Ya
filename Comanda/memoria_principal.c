#include "memoria_principal.h"
#include "comanda.h"

t_list* tabla_marcos_principal;
int cantidad_frames_principal;
t_bitarray* mapa_bits_principal;
pthread_mutex_t mutex;
pthread_mutex_t mutex_mapa_bit;

static t_marco* crear_marco()
{
	t_marco* marco = malloc(sizeof(t_marco));
	return marco;
}

t_marco* obtener_marco_principal(int index)
{
	t_marco* marco;
	pthread_mutex_lock(&mutex);
	marco = list_get(tabla_marcos_principal, index);
	pthread_mutex_unlock(&mutex);
	return marco;
}

int primer_marco_principal_libre()
{
	int index;
	bool encontro = false;

	pthread_mutex_lock(&mutex_mapa_bit);
	for(index=0;!encontro && index<cantidad_frames_principal;index++)
	{
		encontro = bitarray_test_bit(mapa_bits_principal, index);
		if (encontro)
			bitarray_clean_bit(mapa_bits_principal, index);
	}
	pthread_mutex_unlock(&mutex_mapa_bit);

	return encontro ? index-1 : -1;
}

void cargar_marco_principal(t_pagina* pagina)
{
	t_marco* marco_virtual = obtener_marco_virtual(pagina->marco_virtual);
	t_marco* marco_principal = obtener_marco_principal(pagina->marco_principal);

	strcpy (marco_principal->nombre_plato, marco_virtual->nombre_plato);
	marco_principal->cantidad_total = marco_virtual->cantidad_total;
	marco_principal->cantidad_lista = marco_virtual->cantidad_lista;

	pagina->validacion_principal = true;

	log_info(logger, "Se cargo el marco de memoria principal de posicion %d con nombre de plato %s, cantidad lista %d y cantidad total %d", pagina->marco_principal, marco_principal->nombre_plato, marco_principal->cantidad_lista, marco_principal->cantidad_total);

}

static void pagina_modificada(t_pagina* pagina, t_marco* marco_principal)
{
	pagina->uso = true;
	pagina->modificado = true;
	log_info(logger, "Se escribio el marco de memoria principal de posicion %d con nombre de plato %s, ahora tiene cantidad lista %d y cantidad total %d", pagina->marco_principal, marco_principal->nombre_plato, marco_principal->cantidad_lista, marco_principal->cantidad_total);
}

void escribir_marco_principal_guardar_plato(t_pagina* pagina, int cantidad_comida)
{
	t_marco* marco_principal = obtener_marco_principal(pagina->marco_principal);
	marco_principal->cantidad_total = marco_principal->cantidad_total + cantidad_comida;
	pagina_modificada(pagina, marco_principal);
}

void escribir_marco_principal_plato_listo(t_pagina* pagina)
{
	t_marco* marco_principal = obtener_marco_principal(pagina->marco_principal);
	if(pagina->comida_esta_lista)
		{log_info(logger, "De esta comida ya estan listos todos los platos, no se puede pasar un plato listo");
		return;}
	marco_principal->cantidad_lista = marco_principal->cantidad_lista + 1;
	pagina->comida_esta_lista = marco_principal->cantidad_lista == marco_principal->cantidad_total;
	pagina_modificada(pagina, marco_principal);
}

bool comida_esta_lista(t_pagina* pagina)
{
	return pagina->comida_esta_lista;
}

t_datos_estado_pedido* leer_pedido_memoria_principal(t_segmento* segmento_pedido)
{
	void* transfomer(void* pagina_void)
	{
		t_pagina* pagina = pagina_void;

		cargar_desde_swap_si_es_necesario(pagina);

		t_marco* marco_principal = obtener_marco_principal(pagina->marco_principal);
		pagina->uso = true;

		log_info(logger, "Se leyo el marco de memoria principal de posicion %d con nombre de plato %s, cantidad lista %d y cantidad total %d", pagina->marco_principal, marco_principal->nombre_plato, marco_principal->cantidad_lista, marco_principal->cantidad_total);

		t_datos_estado_comida* estado_comida = crear_datos_estado_comida(marco_principal->nombre_plato, marco_principal->cantidad_total, marco_principal->cantidad_lista);

		return 	estado_comida;}

	t_list* platos = list_map(segmento_pedido->tabla_paginas, &transfomer);
	t_estado_pedido estado = segmento_pedido->estado;

	return crear_datos_estado_pedido(estado, platos);
}

int numero_bytes_para_mapa(int cantidad_frames)
{
	float f = cantidad_frames/8;
	float prueba = cantidad_frames/8;
	f = roundf(f);
	if (f < prueba)
		return f+1;
	else return f;
}

void inicializar_memoria_principal()
{
	cantidad_frames_principal = (config_get_int_value(config, "TAMANIO_MEMORIA") / 32 );
	tabla_marcos_principal = list_create();
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_mapa_bit, NULL);

	int num_bytes = numero_bytes_para_mapa(cantidad_frames_principal);
	void* mentira = malloc(num_bytes);
	mapa_bits_principal = bitarray_create_with_mode(mentira, num_bytes, MSB_FIRST);
	int i;
	for (i=0; i < cantidad_frames_principal; i++)
	{
		bitarray_set_bit(mapa_bits_principal, i);
		list_add(tabla_marcos_principal, crear_marco());
	}
	for (i = cantidad_frames_principal; i < bitarray_get_max_bit(mapa_bits_principal); i++)
	{
		bitarray_clean_bit(mapa_bits_principal, i);
	}
}

void vaciar_pagina_memoria_principal(t_pagina* pagina)
{
	sacar_de_paginas_en_memoria(pagina);
	bitarray_set_bit(mapa_bits_principal, pagina->marco_principal);
	log_info(logger, "Se elimina la particion del marco de memoria principal de posicion %d", pagina->marco_principal);
}




