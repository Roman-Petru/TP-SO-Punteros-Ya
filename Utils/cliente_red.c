#include "cliente_red.h"

// ===== Cliente ===== //
t_cliente_red* cliente_crear(t_config* config)
{
	t_cliente_red* cliente = malloc(sizeof(t_cliente_red));
	cliente->config =  config;

	return cliente;
}

static char* cliente_ip(t_cliente_red* cliente, char* servidor)
{
	char* key = malloc(3+strlen(servidor)+1);
	sprintf(key, "IP_%s", servidor);
	char* ip = config_get_string_value(cliente->config, key);
	free(key);
	return ip;
}

static char* cliente_puerto(t_cliente_red* cliente, char* servidor)
{
	char* key = malloc(7+strlen(servidor)+1);
	sprintf(key, "PUERTO_%s", servidor);
	char* puerto = config_get_string_value(cliente->config, key);
	free(key);
	return puerto;
}

void cliente_destruir(t_cliente_red* cliente) {	free(cliente); }

// ===== Enviar Mensaje ===== //

void* cliente_enviar_mensaje(t_cliente_red* cliente, char* servidor, t_codigo_de_operacion codigo_operacion, void* datos)
{
	cliente->socket = socket_crear(cliente_ip(cliente, servidor), cliente_puerto(cliente, servidor));
	t_paquete* paquete = paquete_crear(codigo_operacion, buffer_crear_con_datos(codigo_operacion, datos));
	paquete_enviar(paquete, cliente->socket);
	paquete_destruir(paquete);

	paquete = paquete_recibir(cliente->socket);
	void* pedido = NULL;

	if(paquete_tiene_datos(paquete))
		pedido = paquete_desempaquetar(paquete);

	paquete_destruir(paquete);
	socket_cerrar(cliente->socket);

	return pedido;
}
