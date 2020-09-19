#include "buffer.h"
#include "socket.h"
#include "protocolo.h"

typedef struct
{
	t_codigo_de_operacion codigo_operacion;
	t_buffer* buffer;
}t_paquete;

t_paquete* paquete_crear(t_codigo_de_operacion codigo_operacion, t_buffer* buffer);
void paquete_destruir(t_paquete* paquete);
void paquete_enviar(t_paquete* paquete, int socket);
t_paquete* paquete_recibir(int numero_socket);
void* paquete_desempaquetar(t_paquete* paquete);
