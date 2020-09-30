#ifndef APP_APP
#define APP_APP

#include "interfaz.h"
#include <commons/config.h>
#include <commons/log.h>
#include <semaphore.h>

#include "planificador.h"
#include "repartidor.h"

extern t_config* config_app;
extern t_log* logger_app;
extern sem_t semaforo_app;

typedef enum {
	FIFO = 0,
	SJF = 1,
	HRRN = 2
} ALGORITMO_PLANI;

#endif //APP_APP
