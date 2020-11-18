#ifndef SIND_H_
#define SIND_H_

#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include "../Utils/servidor_red.h"
#include "../Utils/protocolo.h"
#include "../Utils/serializacion.h"
#include "../Utils/consola.h"
#include "metadata.h"
#include "bloques.h"
#include "archivos.h"
#include "interfaz.h"




extern t_log* logger;
extern t_config* config;
extern t_servidor_red* servidor;
extern t_consola* consola;
extern bool hay_que_leer;

#endif
