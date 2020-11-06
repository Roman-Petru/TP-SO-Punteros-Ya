#ifndef APP_H_
#define APP_H_

#include <commons/config.h>
#include <commons/log.h>
#include <semaphore.h>
#include "../Utils/servidor_red.h"
#include "../Utils/cliente_red.h"
#include "gestor_clientes.h"
#include "interrupciones.h"
#include "planificador.h"
#include "repartidor.h"
#include "interfaz.h"

extern t_servidor_red* servidor;
extern t_cliente_red* cliente;
extern t_config* config;
extern t_log* logger;
extern sem_t sem_ciclo;
extern t_list* restaurantes_conectados;
void terminar();

#endif /*APP_H_*/
