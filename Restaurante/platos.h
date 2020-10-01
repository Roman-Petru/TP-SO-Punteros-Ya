#ifndef PLATOS_RES
#define PLATOS_RES

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>


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


typedef struct {
	char* operacion;
	int ciclos;
}t_paso;

typedef struct {
	char* nombre_plato;
	t_list* pasos_receta;
}t_receta;


t_platos_PCB* crear_plato(char* plato, int id_pedido);
void ciclo_plato(t_platos_PCB* plato);
bool avanzar_paso_receta (t_platos_PCB* plato);
void inicializar_diccionario_recetas();

#endif //PLATOS_RES
