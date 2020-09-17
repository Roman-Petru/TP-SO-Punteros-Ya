#include "app.h"


t_config* config;
t_list* lista_repartidores_libres;
int grado_multiprogramacion;
int grado_multiprocesamiento;


void inicializar_repartidores()
{
	config = leer_config("app.config");

	lista_repartidores_libres = list_create();


	char** repartidores = config_get_array_value(config, "REPARTIDORES");
	char** tiempos_de_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	char** frecuencias_de_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");

	lista_repartidores_libres = agregar_repartidores_a_lista_libre(lista_repartidores_libres, repartidores, tiempos_de_descanso, frecuencias_de_descanso);

	grado_multiprogramacion = list_size(lista_repartidores_libres);
	grado_multiprocesamiento = config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO");

}






int main()
{

	inicializar_repartidores();

	while(true)
	{
		//ejecutar_interrupciones();
		planificador_largo_plazo();
		//planificador_corto_plazo();
	//	esperar_fin_de_ciclo();
	}

}

