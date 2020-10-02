#ifndef RESTO
#define RESTO

#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include "planificacionR.h"
#include "platos.h"
#include "interfaz_restaurante.h"

extern t_log* logger_resto;
extern t_config* config_resto;

extern t_list* cola_Resto_NEW;
extern t_list* cola_de_cola_Resto_READY;
extern t_list* cola_Resto_BLOCKED;
extern t_list* cola_de_cola_Resto_EXEC;
extern t_list* cola_Resto_EXIT;

extern t_list* cola_Hornos_READY;
extern t_list* cola_Hornos_EXEC;

extern t_list* lista_afinidades;

extern sem_t semaforo_resto;
extern int cantidad_hornos;
extern int id_PCB;
extern int quantum;


typedef struct{
	char* plato;
	int cantidad_cocineros;
}t_afinidad;

#endif //RESTO
