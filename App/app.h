#ifndef APP_APP
#define APP_APP

#include "repartidores.h"
#include "planificacion.h"
#include <commons/config.h>
#include <commons/log.h>
#include <semaphore.h>

extern t_log* logger_app;
extern float valor_alpha;

typedef enum{
	FIFO = 0,
	SJF = 1,
	HRRN = 2
}ALGORITMO_PLANI;


extern t_config* config;
extern int grado_multiprocesamiento;
extern sem_t semaforo_app;


void inicializar_app();

#endif //APP_APP
