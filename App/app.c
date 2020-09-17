#include "app.h"


t_config* config;
t_list* lista_repartidores_libres;

t_list* cola_NEW;
t_list* cola_EXE;
t_list* cola_READY;
t_list* cola_BLOCKED;
t_list* cola_EXIT;
/*
t_mutex mutex_planif;

void inicializar_repartidores()
{
	config = leer_config("1.config");

	lista_repartidores_libres = list_create();


	char** repartidores = config_get_array_value(config, "REPARTIDORES");
	char** tiempos_de_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	char** frecuencias_de_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");


	lista_repartidores_libres = agregar_repartidores_a_lista_libre(lista_repartidores_libres, repartidores, tiempos_de_descanso, frecuencias_de_descanso);

}


void funcio_hilo(pedido)
{
	while(true)
	{
		espera_mutex(pedido->mutex);
		hace_cosas
		libera_mutex(mutex_planif);libera_mutex(pedido_a_ejecutar->mutex);
	}
}

t_pedido* pedido
{
	t_mutex mutex;
	t_hilo hilo;
}

t_pedido pedido_a_ejecutar;

t_list* interrupciones()tipo_int, datos
		diccionario_get(dic_interrup, tipo_int)(datos);

void algo
{
	hay_alguno(cola_NEW);
}

int main()
{
	while(true)
	{
		elegir_pedido_ejecutar();
		libera_mutex(pedido_a_ejecutar->mutex);
		espera_mutex(mutex_planif);
		chequear_interrupciones();
		sleep(config_get_int_value(config, "CICLO_CPU"));
	}
	inicializar_repartidores();

}
*/
