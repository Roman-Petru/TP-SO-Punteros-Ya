#ifndef APP_APP
#define APP_APP

#include "planificacion.h"
#include <commons/config.h>

typedef enum{
	FIFO = 0,
	SJF = 1,
	HRRN = 2
}ALGORITMO_PLANI;


extern t_config* config;

void inicializar_repartidores();

#endif //APP_APP
