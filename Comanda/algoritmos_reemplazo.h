#ifndef ALG_RE_H
#define ALG_RE_H


#include <commons/collections/list.h>
#include "gestor_tablas.h"
#include "comanda.h"

extern pthread_mutex_t mutex_algoritmos;

int algoritmo_reemplazo(t_pagina* pagina);
void mover_al_final(t_pagina* pagina_buscada);
void sacar_de_paginas_en_memoria(t_pagina* pagina_buscada);


#endif
