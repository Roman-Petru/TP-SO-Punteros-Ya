#include "restaurante.h"

int cantidad_hornos;
t_servidor_red* servidor;
t_cliente_red* cliente_sind;
t_cliente_red* cliente_app;
t_log* logger_resto;
t_config* config_resto;
t_list* lista_afinidades;
t_list* lista_platos;
char* nombre_restaurante;
t_posicion* posicion;
int indice_id_pedido;
t_dictionary* diccionario_recetas;


void inicializar_restaurante()
{

	logger_resto = log_create("resto.log", "RESTAURANTE", true, LOG_LEVEL_INFO);
	config_resto = config_create("restaurante.config");
	nombre_restaurante = config_get_string_value(config_resto, "NOMBRE_RESTAURANTE");

	servidor = servidor_crear("127.0.0.1", config_get_string_value(config_resto, "PUERTO_ESCUCHA"));
	cargar_interfaz();

	cliente_sind = cliente_crear(config_get_string_value(config_resto, "IP_SINDICATO"), config_get_string_value(config_resto, "PUERTO_SINDICATO"));
	cliente_app = cliente_crear(config_get_string_value(config_resto, "IP_APP"), config_get_string_value(config_resto, "PUERTO_APP"));

	inicializar_interrupciones();

	obtener_metadata();
	realizar_handshake_con_app();

	inicializar_planificador();
	diccionario_recetas = dictionary_create();
}

void terminar()
{
	serializacion_finalizar();
	servidor_destruir(servidor);
	log_info(logger_resto, "TERMINE");
	exit(0);
}


int main()

{
	inicializar_restaurante();
	log_info(logger_resto, "Se inicializó el Restaurante");
	//para test/**/


	while (1)
	{
		planificar_corto_plazo();
		ejecutar_ciclo();
		ejecutar_interrupciones();
	}

}
