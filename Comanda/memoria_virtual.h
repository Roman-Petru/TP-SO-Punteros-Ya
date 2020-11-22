#ifndef MEM_VR_H
#define MEM_VR_H


#include "memoria_principal.h"
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <stdint.h>
#include "algoritmos_reemplazo.h"

extern t_bitarray* mapa_bits_virtual;

void inicializar_memoria_virtual();
int primer_marco_virtual_libre();
t_marco* obtener_marco_virtual(int index);
void cargar_marco_virtual(int index, t_pagina* pagina, char* comida);
void escribir_marco_virtual(t_pagina* pagina);
bool cargar_desde_swap_si_es_necesario(t_pagina* pagina);
void vaciar_pagina_memoria_virtual(t_pagina* pagina);


#endif
