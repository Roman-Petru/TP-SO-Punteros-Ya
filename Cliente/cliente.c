#include "cliente.h"

t_config* config;
t_consola* consola;
t_cliente_red* cliente;

int id_pedido;
char* restaurante_seleccionado;
char* cliente_id;
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
	consola = consola_crear("cliente.log", "Cliente");
//	char* string_config = consola_leer("Ingrese el nombre del config deseado: ");
	//config = config_create(string_config);

	config = config_create("cliente.config");
	id_pedido = 0;
	restaurante_seleccionado = NULL;
	hay_que_leer = true;
	cliente_id = config_get_string_value(config, "ID_CLIENTE");

	serializacion_inicializar();
	cliente = cliente_crear(config_get_string_value(config, "IP"), config_get_string_value(config, "PUERTO"));
	 //config_get_string_value(config, "ARCHIVO_LOG")
	cargar_interfaz();
	inicializar_diccionario_estados();
	handshake();
	log_info(consola->logger, "Cliente con id: %s conectado!", cliente_id);
}
void leer_consola()
{
	while(hay_que_leer)
		consola_leer_comando(consola, "Cliente: ");
	terminar_programa();
}


int main()
{
	inicializar();
	leer_consola();

}



