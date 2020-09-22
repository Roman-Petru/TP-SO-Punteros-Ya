#include "protocolo.h"
#include "dictionary_int.h"

typedef struct
{
	char* ip;
	char* puerto;
	int socket;
	pthread_t* hilo_escucha;
	t_dictionary_int* diccionario_operaciones;
}t_cliente_red;

t_cliente_red* cliente_crear(char* ip, char* puerto);
void cliente_destruir(t_cliente_red* cliente);
void cliente_enviar_mensaje(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* datos);
void cliente_agregar_operacion(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* operacion);
