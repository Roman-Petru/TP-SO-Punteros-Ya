#pragma once

#include "memoria_principal.h"
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <stdint.h>


typedef struct{
	uint32_t cantidad_total;
	uint32_t cantidad_lista;
	char nombre_plato [24];
}t_marco;

void inicializar_memoria_virtual();
bool cargar_desde_swap_si_es_necesario(t_pagina* pagina);
