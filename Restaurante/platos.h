#ifndef PLATOS_RES
#define PLATOS_RES

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "../Utils/receta.h"

typedef enum{
	NEW = 0,
	READY = 1,
	EXEC = 2,
	BLOCKED = 3,
	EXIT = 4
}ESTADO_PCB;


typedef struct {
	char* nombre_plato;
	int id_pedido;
	int id_PCB;
	t_list* pasos_receta_faltantes;
	int ciclos_restantes_paso_actual;
	bool hubo_cambio_operacion;


	int ciclos_ejecutandose;
	ESTADO_PCB estado_pcb;

	pthread_t hilo;
	sem_t mutex;
}t_platos_PCB;



typedef void* (*t_insertador_platos)(char*, int);

typedef struct {
	char* nombre_plato;
	int id_pedido;
}t_para_nuevo_plato;

t_platos_PCB* crear_plato(char* plato, int id_pedido);
void destruir_plato (t_platos_PCB* plato);
void ciclo_plato(t_platos_PCB* plato);
void avanzar_paso_receta (t_platos_PCB* plato);
void cambiar_paso_de_ser_necesario(t_platos_PCB* plato);
void logear_inicio_operacion(t_platos_PCB* plato);
void terminar_plato (t_platos_PCB* plato);
void inicializar_diccionario_recetas();

#endif //PLATOS_RES
