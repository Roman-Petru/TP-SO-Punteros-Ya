#include <pthread.h>
#include <semaphore.h>

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
//	t_list* pasos_receta;
	int paso_actual;

	int ciclos_ejecutandose;
	ESTADO_PCB estado_pcb;

	pthread_t hilo;
	sem_t mutex;
}t_platos_PCB;



t_platos_PCB* crear_plato(char* plato, int id_pedido);
