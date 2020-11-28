#include "comanda.h"
#include <netdb.h>

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
	servidor = servidor_crear("10.108.64.3", config_get_string_value(config, "PUERTO_ESCUCHA"));
	cargar_interfaz();

	inicializar_memoria_principal();
	inicializar_memoria_virtual();
	inicializar_gestor_tablas();
}


int main()
{
	inicializar_comanda();
	log_info(logger, "Se inicializo la comanda, esperando conecciones de la app");
	while(true)
		sleep(1000000);
}
