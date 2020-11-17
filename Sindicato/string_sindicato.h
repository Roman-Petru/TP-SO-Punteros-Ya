#ifndef STRING_SINDICATO_H_
#define STRING_SINDICATO_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "../Utils/protocolo.h"
#include "bloques.h"

typedef struct {
	char* estado;
	t_list* platos;
	int precio;
}t_pedido_sind;

bool mod_string_guardar_plato(t_datos_para_guardar* datos_para_bloques, t_guardar_plato* datos_a_guardar);

#endif
