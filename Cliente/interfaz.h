#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include "cliente.h"
#include "../Utils/posicion.h"
#include "../Utils/protocolo.h"
#include "../Utils/serializacion.h"
#include <stdbool.h>
#include <stdio.h>

void cargar_interfaz();
void seleccionar_modulo();
void handshake_con_app();

#endif /*INTERFAZ_H_*/