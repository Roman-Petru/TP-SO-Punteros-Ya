#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include "pedido.h"
#include "planificador.h"
#include "interrupciones.h"
#include "gestor_pedidos.h"
#include "gestor_clientes.h"
#include "gestor_restaurantes.h"
#include "../Utils/serializacion.h"
#include "../Utils/protocolo.h"
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>



void cargar_interfaz();

#endif /*INTERFAZ_H_*/
