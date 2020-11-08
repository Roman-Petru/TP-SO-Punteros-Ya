#include "interfaz.h"
#include "app.h"


static t_respuesta* conexion_cliente(t_posicion* posicion_cliente)
{
	agregar_cliente(posicion_cliente);
	log_info(logger, "Se conecto un cliente Pos: (%d, %d).", posicion_cliente->x, posicion_cliente->y);
	return respuesta_crear(CONEXION_CLIENTE_RESPUESTA, (void*) id_cliente, false);
}

//========== INTERFAZ ==========//
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

/*Obtener Restaurante*/

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

static t_respuesta* agregar_plato(t_agregar_plato* datos)
{
	char* restaurante = pedido_obtener_restaurante(datos->id_pedido);

	if(restaurante_esta_conectado(restaurante))
	{
		bool op_ok = cliente_enviar_mensaje(cliente, "RESTAURANTE", AGREGAR_PLATO, &datos);

		if(!op_ok)
			return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) false, false);
	}

	t_guardar_plato* datos_b = crear_datos_agregar_plato(datos->id_pedido, 1, datos->plato, restaurante);// VER CANTIDAD
	return respuesta_crear(AGREGAR_PLATO_RESPUESTA, cliente_enviar_mensaje(cliente, "COMANDA", GUARDAR_PLATO, &datos_b), false);
}

static t_respuesta* plato_listo(t_plato_listo* datos)
{
	bool op_ok = cliente_enviar_mensaje(cliente, "COMANDA", PLATO_LISTO, datos);

	t_estado_pedido* pedido = cliente_enviar_mensaje(cliente, "COMANDA", OBTENER_PEDIDO, crear_datos_pedido(datos->id_pedido, datos->restaurante));
	pedido_actualizar_estado(datos->id_pedido, pedido);

	return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) op_ok, false);
}

static t_respuesta* confirmar_pedido(t_datos_pedido* datos)
{
	//t_estado_pedido* estado = cliente_enviar_mensaje(cliente, "COMANDA", OBTENER_PEDIDO, crear_datos_pedido(datos->id_pedido, datos->restaurante));
	cliente_enviar_mensaje(cliente, "COMANDA", OBTENER_PEDIDO, crear_datos_pedido(datos->id_pedido, datos->restaurante));

	bool op_ok = cliente_enviar_mensaje(cliente, "RESTAURANTE", CONFIRMAR_PEDIDO, datos);

	if(!op_ok)
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);

	t_pedido* pedido = pedido_crear(datos->id_pedido, restaurante_obtener_posicion(datos->restaurante), cliente_obtener_posicion(pedido_obtener_cliente(datos->id_pedido)), false);
	meter_en_cola(pedido, NEW, A_NEW);

	//ACTUALIZAR PEDIDO EN COMANDA
	//cliente_enviar_mensaje(cliente, "COMANDA", OBTENER_PEDIDO, crear_datos_pedido(datos->id_pedido, datos->restaurante));

	return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) true, false);
}

static t_respuesta* consultar_pedido(uint32_t id_pedido)
{
	char* restaurante = pedido_obtener_restaurante(id_pedido);
	t_estado_pedido* estado = cliente_enviar_mensaje(cliente, "COMANDA", OBTENER_PEDIDO, crear_datos_pedido(id_pedido, restaurante));

	return respuesta_crear(CONSULTAR_PEDIDO_RESPUESTA, crear_datos_consultar_pedido(restaurante, estado->estado, estado->platos), true);
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
	servidor_agregar_operacion(servidor, CONSULTAR_RESTAURANTES, &consultar_restaurantes);
	servidor_agregar_operacion(servidor, SELECCIONAR_RESTAURANTE, &seleccionar_restaurante);
	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
	servidor_agregar_operacion(servidor, CREAR_PEDIDO, &crear_pedido);
	servidor_agregar_operacion(servidor, AGREGAR_PLATO, &agregar_plato);
	servidor_agregar_operacion(servidor, PLATO_LISTO, &plato_listo);
	servidor_agregar_operacion(servidor, CONFIRMAR_PEDIDO, &confirmar_pedido);
	servidor_agregar_operacion(servidor, CONSULTAR_PEDIDO, &consultar_pedido);
}

