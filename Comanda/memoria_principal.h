#pragma once

#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <stdint.h>


typedef struct{
	uint32_t cantidad_total;
	uint32_t cantidad_lista;
	char nombre_plato [24];
}t_marco;


void inicializar_memoria_principal();

