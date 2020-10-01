#ifndef APP_H_
#define APP_H_

#include <commons/config.h>
#include <commons/log.h>
#include <semaphore.h>

#include "interrupciones.h"
#include "planificador.h"
#include "repartidor.h"
#include "interfaz.h"

extern t_config* config_app;
extern t_log* logger_app;
extern sem_t semaforo_app;

#endif /*APP_H_*/
