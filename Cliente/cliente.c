#include "cliente.h"

t_config* config;
t_consola* consola;
t_cliente_red* cliente;
t_servidor_red* servidor;

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


/*FINALIZAR PEDIDO*/
static t_respuesta* finalizar_pedido(t_datos_pedido* datos)
{
	consola_log(consola, "Se finalizo el pedido correctamente!.");
	return respuesta_crear(FINALIZAR_PEDIDO_RESPUESTA, (void*) true , false);
}


static void cargar_interfaz_serv(){

servidor_agregar_operacion(servidor, FINALIZAR_PEDIDO_CLIENTE, &finalizar_pedido);

}

static void inicializar()
{
	consola = consola_crear("cliente.log", "Cliente");
	char* string_config = consola_leer("Ingrese el nombre del config deseado: ");

	config = config_create(string_config);

	id_pedido = 0;
	restaurante_seleccionado = NULL;
	hay_que_leer = true;
	cliente_id = config_get_string_value(config, "ID_CLIENTE");
	servidor = servidor_crear(config_get_string_value(config, "IP_SERVIDOR"), config_get_string_value(config, "PUERTO_ESCUCHA"));
	serializacion_inicializar();
	cargar_interfaz_serv();

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



