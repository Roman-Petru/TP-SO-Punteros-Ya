#include "interfaz.h"
#include "app.h"

static t_respuesta* consultar_restaurantes()
{
	log_info(logger, "Me consultaron las Restaurantes.");

	t_list* restaurantes = list_create();

	//TODO App: consultar_restaurantes  //Descomentar cuando este hecha la lista
	//if(hay_restaurantes)
	//	cargar_restaurantes(restaurantes);
	//else
		list_add(restaurantes, "Resto_Default");

	return respuesta_crear(CONSULTAR_RESTAURANTES_RESPUESTA, restaurantes, true);
}

static t_respuesta* seleccionar_restaurante(char* restaurante)
{
	//TODO App: seleccionar_restaurante
	if(strcmp(restaurante, "Resto_Default")==0) // || esta_conectado(restaurante)
	{
		log_info(logger, "Seleccionaron Restaurante %s.", restaurante);
		return respuesta_crear(SELECCIONAR_RESTAURANTE_RESPUESTA, (void*) true, false);
	}
	else
	{
		log_info(logger, "Error al intentar seleccionar Restaurante %s.", restaurante);
		return respuesta_crear(SELECCIONAR_RESTAURANTE_RESPUESTA, (void*) false, false);
	}
}

static void platos_default(t_list* platos)
{
	char** platos_default = config_get_array_value(config, "PLATOS_DEFAULT");
	for (int i=0; platos_default[i] != NULL; i++)
		list_add(platos, platos_default[i]);
	free(platos_default);
}

static t_respuesta* consultar_platos(char* restaurante)
{
	log_info(logger, "Se consulto platos de Restaurante %s.", restaurante);
	t_list* platos = list_create();

	//TODO App: consultar_platos
	//if(esta_conectado(restaurante))
	//	consultar platos a restaurante y cargar en t_list* platos
	//else
		platos_default(platos);
	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, platos, true);
}


static t_respuesta* operacion_terminar()
{
	agregar_interrupcion(TERMINAR_APP, NULL);
	return respuesta_crear(TERMINAR, NULL, false);
}


void cargar_interfaz()
{
	servidor_agregar_operacion(servidor, TERMINAR, &operacion_terminar);
	servidor_agregar_operacion(servidor, CONSULTAR_RESTAURANTES, &consultar_restaurantes);
	servidor_agregar_operacion(servidor, SELECCIONAR_RESTAURANTE, &seleccionar_restaurante);
	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);

}
