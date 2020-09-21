#include "app.h"


t_config* config;
t_list* lista_repartidores_libres;
int grado_multiprogramacion;
int grado_multiprocesamiento;
float valor_alpha;

sem_t semaforo_app;

t_log* logger_app;

void inicializar_app()
{
	config = config_create("app.config");

	lista_repartidores_libres = list_create();


	char** repartidores = config_get_array_value(config, "REPARTIDORES");
	char** tiempos_de_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	char** frecuencias_de_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");

	lista_repartidores_libres = agregar_repartidores_a_lista_libre(lista_repartidores_libres, repartidores, tiempos_de_descanso, frecuencias_de_descanso);

	grado_multiprogramacion = list_size(lista_repartidores_libres);
	grado_multiprocesamiento = config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO");
	char* token = config_get_string_value(config, "ALPHA");
	valor_alpha = convertir_string_en_float(token);

	sem_init (&(semaforo_app), 0, 0);


	logger_app = log_create("app.log", "APP", true, LOG_LEVEL_INFO);


	cola_NEW = list_create();
	cola_READY = list_create();
	cola_BLOCKED = list_create();
	cola_EXEC = list_create();
	cola_EXIT = list_create();

	//inicializar_diccionario_acciones();
	inicializar_diccionario_colas();
}

t_pedido* crear_pedido_con_resto_default(int id)
{
	t_posicion* posicion_de_restaurante = malloc(sizeof(t_posicion));
	posicion_de_restaurante->x = config_get_int_value(config, "POSICION_REST_DEFAULT_X");
	posicion_de_restaurante->y = config_get_int_value(config, "POSICION_REST_DEFAULT_Y");


	t_posicion* posicion_de_cliente = malloc(sizeof(t_posicion));
	posicion_de_cliente->x = 7;
	posicion_de_cliente->y = 4;

	t_pedido* nuevo_pedido = crear_pedido(id, posicion_de_restaurante, posicion_de_cliente, true);

	return nuevo_pedido;
}

int main()
{

	inicializar_app();
	list_add(cola_NEW, crear_pedido_con_resto_default(343));
	list_add(cola_NEW, crear_pedido_con_resto_default(827));
	list_add(cola_NEW, crear_pedido_con_resto_default(7777));
	list_add(cola_NEW, crear_pedido_con_resto_default(948765));
	list_add(cola_NEW, crear_pedido_con_resto_default(204));

	while(true)
	{
		planificar_largo_plazo();
		planificar_corto_plazo();
		ejecutar_ciclo();

		//ejecutar_interrupciones();
	}

}

