#ifndef COMANDA_H_
#define COMANDA_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../Utils/servidor_red.h"
#include "../Utils/protocolo.h"
#include "interfaz.h"
#include "memoria_principal.h"
#include "memoria_virtual.h"
#include "gestor_tablas.h"

extern t_servidor_red* servidor;
extern t_log* logger;
extern t_config* config;
extern t_list* tabla_restaurantes;
extern t_list* paginas_en_memoria;
extern int puntero_a_paginas;

#endif

