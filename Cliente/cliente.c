#include "cliente.h"

t_config* config;
t_consola* consola;
t_cliente_red* cliente;
t_cliente_red* cliente2;
t_log* logger_finalizar_pedido;

int id_pedido;
char* restaurante_seleccionado;
bool hay_que_leer;

static void terminar_programa()
{
	config_destroy(config);
	consola_destruir(consola);
	cliente_destruir(cliente);
	serializacion_finalizar();
}

static void inicializar()
{
	config = config_create("cliente.config");

	id_pedido = 0;
	restaurante_seleccionado = NULL;
	hay_que_leer = true;

	serializacion_inicializar();
	cliente = cliente_crear(config_get_string_value(config, "IP"), config_get_string_value(config, "PUERTO"));
	cliente2 = cliente_crear(config_get_string_value(config, "IP"), config_get_string_value(config, "PUERTO"));
	consola = consola_crear("cliente.log", "Cliente"); //config_get_string_value(config, "ARCHIVO_LOG")
	logger_finalizar_pedido = log_create("clienteFinalizar.log", "ClienteFin", true, LOG_LEVEL_INFO);
	cargar_interfaz();

	handshake();
}

int main()
{
	inicializar();
	while(hay_que_leer)
		consola_leer_comando(consola, "Cliente: ");
	terminar_programa();
}



