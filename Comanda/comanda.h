#ifndef COMANDA_H_
#define COMANDA_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../Utils/servidor_red.h"
#include "../Utils/protocolo.h"
#include "interfaz.h"
#include "memoria_principal.h"
#include "gestor_tablas.h"

extern t_servidor_red* servidor;
extern t_log* logger;
extern t_config* config;
extern t_list* tabla_restaurantes;

typedef struct{
	char* restaurante;
	t_list* tabla_segmentos;
	pthread_mutex_t mutex_tabla_segmentos;
}t_restaurante;

typedef struct{
	int id_pedido;
	t_estado_pedido estado;
	t_list* tabla_paginas;
}t_segmento;

typedef struct{
	int id;
	char* comida;
	int marco_principal;
	int marco_virtual;
	bool esta_lista;
	bool uso;
	bool modificado;
	bool validacion_principal;
	bool validacion_virtual;
}t_pagina;


#endif

