#include "app.h"

t_servidor_red* servidor;
t_config* config;
t_log* logger;
sem_t sem_ciclo;

static void inicializar()
{
	logger = log_create("app.log", "APP", true, LOG_LEVEL_INFO); //config_get_string_value(config_app, "ARCHIVO_LOG")
	config = config_create("app.config");

	cargar_repartidores();
	inicializar_planificador();
	inicializar_interrupciones();
	serializacion_inicializar();
	servidor = servidor_crear("127.0.0.1", config_get_string_value(config, "PUERTO_ESCUCHA"));
	cargar_interfaz();
	sem_init (&(sem_ciclo), 0, 0);
}

void terminar()
{
	//serializacion_finalizar();
	servidor_destruir(servidor);
	log_info(logger, "TERMINE");
	exit(0);
}
int main()
{
	inicializar();
	recibir_pedidos_default(3); // Para Test

	while(true)
	{
		planificar_largo_plazo();
		planificar_corto_plazo();
		ejecutar_ciclo();
		ejecutar_interrupciones();
	}
}
