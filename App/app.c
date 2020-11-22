#include "app.h"

t_servidor_red* servidor;
t_cliente_red* cliente_comanda;
t_config* config;
t_log* logger;
sem_t sem_ciclo;

static int cant_ciclo;

static void inicializar()
{
	logger = log_create("app.log", "APP", true, LOG_LEVEL_INFO); //config_get_string_value(config_app, "ARCHIVO_LOG")
	config = config_create("app.config");

	cargar_repartidores();
	inicializar_planificador();
	inicializar_interrupciones();
	serializacion_inicializar();
	servidor = servidor_crear("127.0.0.1", config_get_string_value(config, "PUERTO_ESCUCHA"));
	cliente_comanda = cliente_crear(config_get_string_value(config, "IP_COMANDA"), config_get_string_value(config, "PUERTO_COMANDA"));
	cargar_interfaz();
	sem_init (&(sem_ciclo), 0, 0);
	cant_ciclo = 0;

	inicializar_gestor_restaurantes();
	inicializar_gestor_clientes();
	inicializar_gestor_pedidos();
}

/*static void comiezo_ciclo()
{
	cant_ciclo++;
	//if(cant_ciclo == 100)
	//	terminar();
	//log_info(logger, "===== INSTANTE %d =====", cant_ciclo);
}*/

void terminar()
{
	serializacion_finalizar();
	servidor_destruir(servidor);
	finalizar_gestor_clientes();
	finalizar_gestor_pedidos();
	log_info(logger, "TERMINE");
	exit(0);
}

int main()
{
	inicializar();
	log_info(logger, "Se inicializo la app, esperando conecciones de restaurantes y clientes");

	while(true)
	{
		//comiezo_ciclo();
		planificar_largo_plazo();
		planificar_corto_plazo();
		ejecutar_ciclo();
		ejecutar_interrupciones();
	}
}
