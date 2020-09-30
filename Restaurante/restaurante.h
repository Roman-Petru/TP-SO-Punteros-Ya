#ifndef RESTO
#define RESTO

#include <commons/collections/list.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include "planificacionR.h"
#include "platos.h"


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

typedef struct{
	char* plato;
	int cantidad_cocineros;
}t_afinidad;

#endif //RESTO
