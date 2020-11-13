#include "comanda.h"

t_servidor_red* servidor;
t_log* logger;
t_config* config;
t_list* tabla_restaurantes;
t_list* paginas_en_memoria;

int puntero_a_paginas;

void inicializar_comanda(){
	logger = log_create("comanda.log", "COMANDA", true, LOG_LEVEL_INFO);
	config = config_create("comanda.config");

	serializacion_inicializar();
	servidor = servidor_crear("127.0.0.1", config_get_string_value(config, "PUERTO_ESCUCHA"));
	cargar_interfaz();

	inicializar_memoria_principal();
	inicializar_memoria_virtual();
	inicializar_gestor_tablas();
}


int main()
{
	inicializar_comanda();

	while(true)
		sleep(1000000);
}
