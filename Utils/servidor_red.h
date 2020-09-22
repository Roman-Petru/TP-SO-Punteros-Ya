#include "dictionary_int.h"
#include "protocolo.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct
{
	int socket;
	pthread_t hilo_escucha;
	t_dictionary_int* diccionario_operaciones;
}t_servidor_red;

t_servidor_red* servidor_crear(char* ip, char* puerto);
void servidor_destruir(t_servidor_red* servidor);
void servidor_agregar_operacion(t_servidor_red* servidor, t_codigo_de_operacion codigo_operacion, void* operacion);
