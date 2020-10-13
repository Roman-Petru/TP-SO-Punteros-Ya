#ifndef CLIENTE_H_
#define CLIENTE_H_

#include "../Utils/serializacion.h"
#include "../Utils/cliente_red.h"
#include "../Utils/consola.h"
#include <commons/config.h>

#include "interfaz.h"
#include "operaciones.h"


extern t_config* config;
extern t_consola* consola;
extern t_cliente_red* cliente;
extern int id_pedido;
extern char* restaurante_seleccionado;
extern bool hay_que_leer;

#endif /*CLIENTE_H_*/


