#include "app.h"


t_config* config;
t_list* lista_repartidores_libres;

void inicializar_repartidores()
{
	config = leer_config("1.config");

	lista_repartidores_libres = list_create();


	char** repartidores = config_get_array_value(config, "REPARTIDORES");
	char** tiempos_de_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	char** frecuencias_de_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");


	lista_repartidores_libres = agregar_repartidores_a_lista_libre(lista_repartidores_libres, repartidores, tiempos_de_descanso, frecuencias_de_descanso);

}






int main()
{

	inicializar_repartidores();

}

