#include "comanda.h"

t_servidor_red* servidor;
t_log* logger;
t_config* config;
t_list* tabla_restaurantes;

void inicializar_comanda(){
	tabla_restaurantes = list_create();

	logger = log_create("comanda.log", "COMANDA", true, LOG_LEVEL_INFO);
	config = config_create("comanda.config");

	serializacion_inicializar();
	servidor = servidor_crear("127.0.0.1", config_get_string_value(config, "PUERTO_ESCUCHA"));
	cargar_interfaz();

	inicializar_memoria_principal();
}


int main()
{
	inicializar_comanda();

}
