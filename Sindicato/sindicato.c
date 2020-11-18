#include "sindicato.h"



static void cargar_interfaz_consola();

t_servidor_red* servidor;
t_log* logger;
t_config* config;
t_consola* consola;
bool hay_que_leer;

void inicializar_sindicato()
{
	logger = log_create("sindicato.log", "SINDICATO", true, LOG_LEVEL_INFO);
	config = config_create("sindicato.config");

	serializacion_inicializar();
	servidor = servidor_crear("127.0.0.1", config_get_string_value(config, "PUERTO_ESCUCHA"));
	cargar_interfaz();

	consola = consola_crear("sindicato_consola.log", "Sindicato");
	cargar_interfaz_consola();
	hay_que_leer = true;

	inicializar_metadata();
	inicializar_bloques();
	inicializar_archivos();
}


int main()
{
	inicializar_sindicato();

	log_info(logger, "Sindicato inicializado");

		while(hay_que_leer)
			consola_leer_comando_sindicato(consola, "Sindicato: ");

}



static void cargar_interfaz_consola()
{
	consola_agregar_comando(consola, "CrearReceta", &crear_receta);
	consola_agregar_comando(consola, "CrearRestaurante", &crear_restaurante);
}

