#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include "cliente.h"
#include "../Utils/posicion.h"
#include "../Utils/protocolo.h"
#include "../Utils/serializacion.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>



void cargar_interfaz();
void handshake();

#endif /*INTERFAZ_H_*/
