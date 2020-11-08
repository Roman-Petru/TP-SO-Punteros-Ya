#include "interfaz.h"
#include "app.h"

//========== INTERFAZ ==========//
static t_respuesta* conexion_cliente(t_posicion* posicion_cliente)
{
	agregar_cliente(posicion_cliente);
	log_info(logger, "Se conecto un cliente Pos: (%d, %d).", posicion_cliente->x, posicion_cliente->y);
	return respuesta_crear(CONEXION_CLIENTE_RESPUESTA, (void*) id_cliente, false);
}

static t_respuesta* handshake_restaurante_app(t_handshake_resto_app* datos)
{
	char puerto[6];
	sprintf(puerto,"%d",datos->puerto);

	if (!restaurante_esta_conectado(datos->restaurante))
		{
		agregar_restaurante(datos->restaurante, puerto, datos->posicion);
		log_info(logger, "Se conecto el restaurante %s con posicion X: %d, Y: %d.", datos->restaurante, datos->posicion->x, datos->posicion->y);
		}

	return respuesta_crear(HANDSHAKE_RESTO_APP_RESPUESTA, (void*) true, false);

}




static t_respuesta* consultar_restaurantes()
{
	log_info(logger, "Me consultaron las Restaurantes.");

	if(hay_restaurantes_conectados())
		return respuesta_crear(CONSULTAR_RESTAURANTES_RESPUESTA, (void*) obtener_restaurantes_conectados(), true);

	t_list* resto_default = list_create();
	list_add(resto_default, "Resto_Default");

	return respuesta_crear(CONSULTAR_RESTAURANTES_RESPUESTA, resto_default, true);
}

static t_respuesta* seleccionar_restaurante(t_datos_seleccion_restaurante* datos)
{
	char* restaurante = datos->restaurante;

	if(strcmp(restaurante, "Resto_Default")==0 || restaurante_esta_conectado(restaurante))
	{
		log_info(logger, "Seleccionaron Restaurante %s.", restaurante);
		cliente_seleccionar_restaurante(datos->id_cliente, restaurante);
		return respuesta_crear(SELECCIONAR_RESTAURANTE_RESPUESTA, (void*) true, false);
	}
	else
	{
		log_info(logger, "Error al intentar seleccionar Restaurante %s.", restaurante);
		return respuesta_crear(SELECCIONAR_RESTAURANTE_RESPUESTA, (void*) false, false);
	}
}

static t_list* platos_default()
{
	t_list* platos = list_create();

	char** platos_default = config_get_array_value(config, "PLATOS_DEFAULT");
	for (int i=0; platos_default[i] != NULL; i++)
		list_add(platos, platos_default[i]);
	free(platos_default);

	return platos;
}

static t_respuesta* consultar_platos(char* restaurante)
{
	log_info(logger, "Se consulto platos de Restaurante %s.", restaurante);

	if(restaurante_esta_conectado(restaurante))
		return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, cliente_enviar_mensaje(cliente, restaurante, CONSULTAR_PLATOS, NULL), true); //TODO: Intentar editar config

	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, platos_default(), true);
}

static t_respuesta* crear_pedido(int id_cliente)
{
	char* restaurante = cliente_obtener_restaurante(id_cliente);
	int id_pedido;

	if(restaurante_esta_conectado(restaurante))
		id_pedido = (int) cliente_enviar_mensaje(cliente, restaurante, CREAR_PEDIDO, NULL);
	else if(strcmp(restaurante, "Resto_Default")==0)
		id_pedido = generar_id_pedido();
	else
		return respuesta_crear(CREAR_PEDIDO_RESPUESTA, (void*) -1, false);

	t_datos_pedido datos;
	datos.restaurante = restaurante;
	datos.id_pedido = id_pedido;

	bool op_ok = cliente_enviar_mensaje(cliente, "COMANDA", GUARDAR_PEDIDO, &datos);

	if(op_ok)
		return respuesta_crear(CREAR_PEDIDO_RESPUESTA, (void*) id_pedido, false);
	else
		return respuesta_crear(CREAR_PEDIDO_RESPUESTA, (void*) -1, false);
}

static t_respuesta* operacion_terminar()
{
	agregar_interrupcion(TERMINAR_APP, NULL);
	return respuesta_crear(TERMINAR, NULL, false);
}

void cargar_interfaz()
{
	servidor_agregar_operacion(servidor, TERMINAR, &operacion_terminar);
	servidor_agregar_operacion(servidor, CONEXION_CLIENTE, &conexion_cliente);
	servidor_agregar_operacion(servidor, HANDSHAKE_RESTO_APP, &handshake_restaurante_app);
	servidor_agregar_operacion(servidor, CONSULTAR_RESTAURANTES, &consultar_restaurantes);
	servidor_agregar_operacion(servidor, SELECCIONAR_RESTAURANTE, &seleccionar_restaurante);
	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
	servidor_agregar_operacion(servidor, CREAR_PEDIDO, &crear_pedido);
}

