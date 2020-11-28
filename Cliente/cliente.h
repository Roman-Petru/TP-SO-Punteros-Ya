#ifndef CLIENTE_H_
#define CLIENTE_H_

#include "interfaz.h"
#include "../Utils/serializacion.h"
#include "../Utils/cliente_red.h"
#include "../Utils/consola.h"
#include <commons/config.h>
#include "../Utils/servidor_red.h"


extern t_config* config;
extern t_consola* consola;
extern t_cliente_red* cliente;
extern int id;
extern int id_pedido;
extern char* restaurante_seleccionado;
extern char* cliente_id;
extern bool hay_que_leer;
extern t_log* logger_finalizar_pedido;


void leer_consola();

#endif /*CLIENTE_H_*/


