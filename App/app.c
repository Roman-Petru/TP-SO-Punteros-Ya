#include "app.h"

t_log* logger_app;
t_config* config_app;
sem_t semaforo_app;

static void inicializar_app()
{
	logger_app = log_create("app.log", "APP", true, LOG_LEVEL_INFO); //config_get_string_value(config_app, "ARCHIVO_LOG")
	config_app = config_create("app.config");

	cargar_repartidores(config_app);
	inicializar_planificador();
	inicializar_servidor();
	sem_init (&(semaforo_app), 0, 0);
}

int main()
{
	inicializar_app();
	recibir_pedidos_default(3); // Para Test

	while(true)
	{
		planificar_largo_plazo();
		planificar_corto_plazo();
		ejecutar_ciclo();
		//ejecutar_interrupciones();
	}
}
