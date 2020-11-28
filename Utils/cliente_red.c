#include "cliente_red.h"

#define MY_SOCK_PATH "/somepath"
#define handle_error(msg) \
 do { perror(msg); exit(EXIT_FAILURE); } while (0)

// ===== Cliente ===== //
t_cliente_red* cliente_crear(char* ip, char* puerto)
{
	t_cliente_red* cliente = malloc(sizeof(t_cliente_red));
	cliente->ip =  ip;
	cliente->puerto =  puerto;

	return cliente;
}

void cliente_destruir(t_cliente_red* cliente) {	free(cliente); }

// ===== Enviar Mensaje ===== //

void* cliente_enviar_mensaje(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* datos)
{
	cliente->socket = socket_crear(cliente->ip, cliente->puerto);
	if (cliente->socket == -1)
		return NULL;
	t_paquete* paquete = paquete_crear(codigo_operacion, buffer_crear_con_datos(codigo_operacion, datos));
	bool envio_satisfactorio = paquete_enviar(paquete, cliente->socket);
	paquete_destruir(paquete);

	if(!envio_satisfactorio)
		return NULL;

	paquete = paquete_recibir(cliente->socket);
	void* pedido = NULL;

	if(paquete_tiene_datos(paquete))
		pedido = paquete_desempaquetar(paquete);

	paquete_destruir(paquete);
	socket_cerrar(cliente->socket);

	return pedido;
}
/*
t_paquete* paquete_recibir_asincronico(int numero_socket)
{
	t_paquete* paquete;
	t_buffer* buffer;
	size_t tamanio_buffer;
	t_codigo_de_operacion codigo_operacion;

	if (recv(numero_socket, &codigo_operacion, sizeof(t_codigo_de_operacion),  0) == -1)
		return NULL;

	perror("1er Recv Perror: ");

	if (recv(numero_socket, &tamanio_buffer, sizeof(size_t),  0) == -1)
		return NULL;

	perror("2nd Recv Perror: ");
	buffer = buffer_crear(tamanio_buffer);
	if (tamanio_buffer==0)
		return paquete_crear(codigo_operacion, buffer);

	if (recv(numero_socket, buffer->stream, tamanio_buffer,  0) == -1)
		return NULL;
	perror("3rd Recv Perror: ");
	paquete = paquete_crear(codigo_operacion, buffer);

	return paquete;
}

void* cliente_enviar_mensaje_asincronico(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* datos)
{
	cliente->socket = socket_crear(cliente->ip, cliente->puerto);
	if (cliente->socket == -1)
		return NULL;

	struct addrinfo* direccion = NULL;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo hints2 = hints;
	getaddrinfo(cliente->ip, cliente->puerto, &hints2, &direccion);

	struct addrinfo* servinfo = direccion;
	bind(cliente->socket, servinfo->ai_addr, servinfo->ai_addrlen);

	t_paquete* paquete = paquete_crear(codigo_operacion, buffer_crear_con_datos(codigo_operacion, datos));
	bool envio_satisfactorio = paquete_enviar(paquete, cliente->socket);
	paquete_destruir(paquete);

	if(!envio_satisfactorio)
		return NULL;

	paquete = paquete_recibir_asincronico(cliente->socket);
	void* pedido = NULL;

	if(paquete_tiene_datos(paquete))
		pedido = paquete_desempaquetar(paquete);

	paquete_destruir(paquete);
	socket_cerrar(cliente->socket);

	return pedido;
}*/
