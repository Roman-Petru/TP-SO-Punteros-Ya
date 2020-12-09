#ifndef MEM_PR_H
#define MEM_PR_H


#include "../Utils/protocolo.h"
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <stdint.h>
#include "gestor_tablas.h"
#include <math.h>

typedef struct{
	uint32_t cantidad_total;
	uint32_t cantidad_lista;
	char nombre_plato [24];
}t_marco;


void inicializar_memoria_principal();
void cargar_marco_principal(t_pagina* pagina);
int primer_marco_principal_libre();
t_marco* obtener_marco_principal(int index);
void escribir_marco_principal_guardar_plato(t_pagina* pagina, int cantidad_comida);
bool escribir_marco_principal_plato_listo(t_pagina* pagina);
t_datos_estado_pedido* leer_pedido_memoria_principal(t_segmento* segmento_pedido);
bool comida_esta_lista(t_pagina* pagina);
void vaciar_pagina_memoria_principal(t_pagina* pagina);
int numero_bytes_para_mapa(int cantidad_frames);

#endif
