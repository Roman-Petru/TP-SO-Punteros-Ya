#include "protocolo.h"

typedef struct
{
	char* ip;
	char* puerto;
	int socket;
	pthread_t* hilo_escucha;
	t_dictionary_int* diccionario_operaciones;
}t_cliente_red;

typedef void* (*t_operacion_respuesta)();

t_cliente_red* cliente_crear(char* ip, char* puerto);
void cliente_destruir(t_cliente_red* cliente);
void cliente_enviar_mensaje(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* datos);
