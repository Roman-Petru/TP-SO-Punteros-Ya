#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../Utils/servidor_red.h"
#include "memoria_principal.h"
#include "interfaz_comanda.h"

extern t_servidor_red* servidor;
extern t_log* logger;
extern t_config* config;

extern t_list* restaurantes_en_memoria;

typedef struct{
	char* restaurante;
	t_list* tabla_segmentos;
}t_direccion_a_tabla;

typedef struct{
	int id_pedido;
	bool estado;
	t_list* tabla_paginas;
}t_segmento;

typedef struct{
	int* marco;
}t_pagina;




