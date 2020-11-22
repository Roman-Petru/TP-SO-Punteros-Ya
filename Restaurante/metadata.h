#ifndef METH_H_
#define METH_H_

#include "../Utils/protocolo.h"
#include <commons/string.h>
#include <stdbool.h>
#include "restaurante.h"


void obtener_metadata();
bool obtener_recetas(t_list* platos);
bool existe_plato(char* nombre_plato_buscado);

#endif
