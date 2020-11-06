#include "app.h"

t_servidor_red* servidor;
t_cliente_red* cliente;
t_config* config;
t_log* logger;
sem_t sem_ciclo;
t_list* restaurantes_conectados;

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
	cliente = cliente_crear(config);
	cargar_interfaz();
	sem_init (&(sem_ciclo), 0, 0);
	cant_ciclo = 0;

	restaurantes_conectados = list_create();

	inicializar_gestor_clientes();
}

static void comiezo_ciclo()
{
	cant_ciclo++;
	//log_info(logger, "===== INSTANTE %d =====", cant_ciclo);
}

void terminar()
{
	serializacion_finalizar();
	servidor_destruir(servidor);
	log_info(logger, "TERMINE");
	exit(0);
}

int main()
{
	inicializar();
	//recibir_pedidos_default(3); // Para Test

	while(true)
	{
		comiezo_ciclo();
		planificar_largo_plazo();
		planificar_corto_plazo();
		ejecutar_ciclo();
		ejecutar_interrupciones();
	}
}
