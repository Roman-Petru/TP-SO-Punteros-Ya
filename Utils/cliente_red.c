#include "cliente_red.h"

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
void cliente_enviar_mensaje_asincronico(t_cliente_red* cliente, t_codigo_de_operacion codigo_operacion, void* datos)
{


	cliente->socket = socket_crear(cliente->ip, cliente->puerto);
	if (cliente->socket == -1)
		return;
	t_paquete* paquete = paquete_crear(codigo_operacion, buffer_crear_con_datos(codigo_operacion, datos));
	bool envio_satisfactorio = paquete_enviar(paquete, cliente->socket);
	paquete_destruir(paquete);

	if(!envio_satisfactorio)
		return;

	paquete = paquete_recibir(cliente->socket);
	void* pedido = NULL;

	if(paquete_tiene_datos(paquete))
		pedido = paquete_desempaquetar(paquete);

	paquete_destruir(paquete);
	socket_cerrar(cliente->socket);
}*/
