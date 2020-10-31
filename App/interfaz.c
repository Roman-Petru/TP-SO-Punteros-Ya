#include "interfaz.h"
#include "app.h"

//========== RESTAURANTES CONE ==========//
static bool hay_restaurantes_conectados() { return !list_is_empty(restaurantes_conectados); }
static bool esta_conectado(char* restaurante)
{
	bool es_mismo_resto(void* resto) { return strcmp(restaurante, resto)==0; }
	return list_any_satisfy(restaurantes_conectados, &es_mismo_resto);
}


//========== INTERFAZ ==========//
static t_respuesta* consultar_restaurantes()
{
	log_info(logger, "Me consultaron las Restaurantes.");

	if(hay_restaurantes_conectados())
		return respuesta_crear(CONSULTAR_RESTAURANTES_RESPUESTA, restaurantes_conectados, true);

	t_list* resto_default = list_create();
	list_add(resto_default, "Resto_Default");

	return respuesta_crear(CONSULTAR_RESTAURANTES_RESPUESTA, resto_default, true);
}

static t_respuesta* seleccionar_restaurante(char* restaurante)
{
	if(strcmp(restaurante, "Resto_Default")==0 || esta_conectado(restaurante))
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

	if(esta_conectado(restaurante))
		return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, cliente_enviar_mensaje(cliente, "RESTAURANTE", CONSULTAR_PLATOS, NULL), true); //TODO: DUDA IP_PUERTO RESTO

	t_list* platos = list_create();
	platos_default(platos);

	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, platos, true);
}

static t_respuesta* operacion_terminar()
{
	agregar_interrupcion(TERMINAR_APP, NULL);
	return respuesta_crear(TERMINAR, NULL, false);
}

static t_respuesta* guardar_Pedido()
{

}

static t_respuesta* guardar_Plato()
{

}

void cargar_interfaz()
{
	servidor_agregar_operacion(servidor, TERMINAR, &operacion_terminar);
	servidor_agregar_operacion(servidor, CONSULTAR_RESTAURANTES, &consultar_restaurantes);
	servidor_agregar_operacion(servidor, SELECCIONAR_RESTAURANTE, &seleccionar_restaurante);
	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
	servidor_agregar_operacion(servidor,GUARDAR_PEDIDO,&guardar_Pedido);
	servidor_agregar_operacion(servidor,GUARDAR_PLATO,&guardar_Plato);
}

