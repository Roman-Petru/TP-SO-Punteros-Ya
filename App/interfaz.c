#include "interfaz.h"
#include "app.h"

t_servidor_red* servidor_app;

static t_respuesta* consultar_restaurantes()
{
	log_info(logger_app, "Me consultaron las Restaurantes.");

	t_list* restaurantes = list_create();

	//TODO App: consultar_restaurantes
	//if(hay_restaurantes)
	//	cargar_restaurantes(restaurantes);
	//else
		list_add(restaurantes, "Resto_Default");

	return respuesta_crear(CONSULTAR_RESTAURANTES, restaurantes);
}

static t_respuesta* seleccionar_restaurante(char* restaurante)
{
	//TODO App: seleccionar_restaurante
	if(strcmp(restaurante, "Resto_Default")==0) // || esta_conectado(restaurante)
	{
		log_info(logger_app, "Seleccionaron Restaurante %s.", restaurante);
		return respuesta_crear(SELECCIONAR_RESTAURANTE_RESPUESTA, (void*) true);
	}
	else
	{
		log_info(logger_app, "Error al intentar seleccionar Restaurante %s.", restaurante);
		return respuesta_crear(SELECCIONAR_RESTAURANTE_RESPUESTA, (void*) false);
	}
}

static void platos_default(t_list* platos)
{
	char** platos_default = config_get_array_value(config_app, "PLATOS_DEFAULT");
	for (int i=0; platos_default[i] != NULL; i++)
		list_add(platos, platos_default[i]);
	free(platos_default);
}

static t_respuesta* consultar_platos(char* restaurante)
{
	log_info(logger_app, "Se consulto platos de Restaurante %s.", restaurante);
	t_list* platos = list_create();

	//TODO App: consultar_platos
	//if(esta_conectado(restaurante))
	//	consultar platos a restaurante y cargar en t_list* platos
	//else
		platos_default(platos);
	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, platos);
}

static t_respuesta* terminar()
{
	serializacion_finalizar();
	servidor_destruir(servidor_app);
	log_info(logger_app, "TERMINE EL SERVIDOR");
	return respuesta_crear(TERMINAR, NULL);
}

void inicializar_servidor()
{
	serializacion_inicializar();
	servidor_app = servidor_crear("127.0.0.1", config_get_string_value(config_app, "PUERTO_ESCUCHA"));
	servidor_agregar_operacion(servidor_app, TERMINAR, &terminar);
	servidor_agregar_operacion(servidor_app, CONSULTAR_RESTAURANTES, &consultar_restaurantes);
	servidor_agregar_operacion(servidor_app, SELECCIONAR_RESTAURANTE, &seleccionar_restaurante);
	servidor_agregar_operacion(servidor_app, CONSULTAR_PLATOS, &consultar_platos);

	log_info(logger_app, "Servidor listo para recibir al cliente");
}
