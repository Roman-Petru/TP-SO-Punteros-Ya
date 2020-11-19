#ifndef STRING_SINDICATO_H_
#define STRING_SINDICATO_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "../Utils/protocolo.h"
#include "../Utils/config_string.h"
#include "bloques.h"

typedef struct {
	char* estado;
	t_list* platos;
	int precio;
}t_pedido_sind;


t_pedido_sind* desglosar_pedido (char* pedido_en_string);
bool mod_string_guardar_plato(t_datos_para_guardar* datos_para_bloques, t_guardar_plato* datos_a_guardar);
bool mod_string_confirmar_pedido(t_datos_para_guardar* datos_para_bloques);
t_datos_estado_pedido* modificar_estructura_pedido(t_pedido_sind* pedido);

#endif
