#include "interfaz.h"
#include "app.h"

static t_respuesta* handshake_cliente()
{
	return respuesta_crear(HANDSHAKE_CLIENTE_RESPUESTA, (void*) APP, false);
}

static t_respuesta* conexion_cliente(t_datos_cliente* datos)
{
	agregar_cliente(datos);
	log_info(logger, "Se conecto un cliente Id: %s Pos: (%d, %d).", datos->id_cliente, datos->posicion->x, datos->posicion->y);
	return respuesta_crear(CONEXION_CLIENTE_RESPUESTA, (void*) true, false);
}

//========== INTERFAZ ==========//
static t_respuesta* handshake_restaurante_app(t_handshake_resto_app* datos)
{


	if (!restaurante_esta_conectado(datos->restaurante))
		{
		agregar_restaurante(datos->restaurante, datos->posicion, datos->ip, datos->puerto);
		log_info(logger, "Se conecto el restaurante %s con posicion X: %d, Y: %d.", datos->restaurante, datos->posicion->x, datos->posicion->y);
		}

	return respuesta_crear(HANDSHAKE_RESTO_APP_RESPUESTA, (void*) true, false);
}

/*CONSULTAR RESTAURANTES*/
static t_respuesta* consultar_restaurantes()
{
	log_info(logger, "Me consultaron las Restaurantes.");

	if(hay_restaurantes_conectados())
		return respuesta_crear(CONSULTAR_RESTAURANTES_RESPUESTA, (void*) obtener_restaurantes_conectados(), true);

	t_list* resto_default = list_create();
	list_add(resto_default, "Resto_Default");

	return respuesta_crear(CONSULTAR_RESTAURANTES_RESPUESTA, resto_default, true);
}

/*SELECCIONAR RESTAURANTE*/
static t_respuesta* seleccionar_restaurante(t_datos_seleccion_restaurante* datos)
{
	char* restaurante = datos->restaurante;

	if(es_resto_default(restaurante) || restaurante_esta_conectado(restaurante))
	{
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

/*CONSULTAR PLATOS*/
static t_respuesta* consultar_platos(char* restaurante)
{
	log_info(logger, "Se consulto platos de Restaurante %s.", restaurante);

	if(!es_resto_default(restaurante) && restaurante_esta_conectado(restaurante))
		return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, cliente_enviar_mensaje(restaurante_obtener_cliente(restaurante), CONSULTAR_PLATOS, NULL), true);

	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, platos_default(), true);
}

/*CREAR PEDIDO*/
static t_respuesta* crear_pedido(char* id_cliente)
{
	char* restaurante = cliente_obtener_restaurante(id_cliente);
	int id_pedido;

	if(es_resto_default(restaurante))
		id_pedido = generar_id_pedido();
	else if(restaurante_esta_conectado(restaurante))
		id_pedido = (int) cliente_enviar_mensaje(restaurante_obtener_cliente(restaurante), CREAR_PEDIDO, NULL);
	else
		{log_info(logger, "No se selecciono un restaurante valido para crear el pedido");
		return respuesta_crear(CREAR_PEDIDO_RESPUESTA, (void*) -1, false);}

	t_datos_pedido* datos = crear_datos_pedido(id_pedido, restaurante);
	bool op_ok = cliente_enviar_mensaje(cliente_comanda, GUARDAR_PEDIDO, datos);

	//Retorna el ID del pedido al Cliente que solicitó el pedido.
	if(!op_ok)
		return respuesta_crear(CREAR_PEDIDO_RESPUESTA, (void*) -1, false);

	vincular_pedido(id_pedido, id_cliente, restaurante);
	return respuesta_crear(CREAR_PEDIDO_RESPUESTA, (void*) id_pedido, false);
}

static bool es_plato_default(char* plato)
{
	t_list* platos = platos_default();
	bool es_mismo_plato(void* plato_default) { return strcmp(plato_default, plato)==0; }
	bool es_default = list_any_satisfy(platos, (void*) &es_mismo_plato);
	destruir_lista_string(platos);

	return  es_default;
}

/*AGREGAR PLATO*/
static t_respuesta* agregar_plato(t_agregar_plato* datos)
{
	char* restaurante = pedido_obtener_restaurante(datos->id_pedido);
	if (restaurante == NULL)
		{log_info(logger, "No hay restaurante o no hay id pedido para este cliente");
		return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) false, false);}
	bool op_ok = false;


	if(es_resto_default(restaurante))
		op_ok = es_plato_default(datos->plato);
	else if(restaurante_esta_conectado(restaurante))
		op_ok = cliente_enviar_mensaje(restaurante_obtener_cliente(restaurante), AGREGAR_PLATO, datos);



	if(!op_ok)
		return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) false, false);

	t_guardar_plato* datos_comanda = crear_datos_guardar_plato(datos->id_pedido, 1, datos->plato, restaurante);
	op_ok = cliente_enviar_mensaje(cliente_comanda, GUARDAR_PLATO, datos_comanda);

	//destruir(GUARDAR_PLATO, datos_comanda);
	return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) op_ok, false);
}

/*PLATO LISTO*/
static t_respuesta* plato_listo(t_guardar_plato* datos)
{
	bool op_ok = cliente_enviar_mensaje(cliente_comanda, PLATO_LISTO, datos);

	if(!op_ok)
		return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);
	t_datos_estado_pedido* pedido = cliente_enviar_mensaje(cliente_comanda, OBTENER_PEDIDO, crear_datos_pedido(datos->id_pedido, datos->restaurante));

	if(pedido->estado == ERROR_ESTADO)
		return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);
	//pedido_actualizar_estado(datos->id_pedido, pedido->estado);

	if(pedido->estado == TERMINADO)
		agregar_interrupcion(PASAR_A_LISTO, (int*) datos->id_pedido);

	destruir(OBTENER_PEDIDO_RESPUESTA, pedido);
	return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) true, false);
}

/*CONFIRMAR PEDIDOs*/
static t_respuesta* confirmar_pedido(t_datos_pedido* datos)
{
	//cliente_enviar_mensaje(cliente_comanda, OBTENER_PEDIDO, crear_datos_pedido(datos->id_pedido, datos->restaurante));
	bool op_ok = true;
	if(!es_resto_default(datos->restaurante) && restaurante_esta_conectado(datos->restaurante))
		op_ok = cliente_enviar_mensaje(restaurante_obtener_cliente(datos->restaurante), CONFIRMAR_PEDIDO, datos);

	if(!op_ok)
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);

	t_pedido* pedido = pedido_crear(datos->id_pedido);

	//TODO: Enviar mensaje Confirmar Pedido a Cliente
	op_ok = cliente_enviar_mensaje(cliente_comanda, CONFIRMAR_PEDIDO, datos);

	if (op_ok)
		agregar_interrupcion(NUEVO_PEDIDO, pedido);

	return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) op_ok , false);
}

/*CONSULTAR PEDIDO*/
static t_respuesta* consultar_pedido(uint32_t id_pedido)
{
	char* restaurante = pedido_obtener_restaurante(id_pedido);

	t_datos_estado_pedido* estado = cliente_enviar_mensaje(cliente_comanda, OBTENER_PEDIDO, crear_datos_pedido(id_pedido, restaurante));

	if (estado == NULL)
		{
		t_estado_pedido estado_n = ERROR_ESTADO;
		t_list* nuevos_pl = list_create();
		return respuesta_crear(CONSULTAR_PEDIDO_RESPUESTA, crear_datos_consultar_pedido(restaurante, estado_n, nuevos_pl), false);

		}
	t_estado_pedido estado_n = estado->estado;
	t_list* nuevos_pl = list_duplicate(estado->platos);

	//destruir(OBTENER_PEDIDO_RESPUESTA, estado);
	return respuesta_crear(CONSULTAR_PEDIDO_RESPUESTA, crear_datos_consultar_pedido(restaurante, estado_n, nuevos_pl), false);
}

/*FINALIZAR PEDIDO
static t_respuesta* finalizar_pedido(t_datos_pedido* datos)
{
	sem_t* sincronizador = pedido_obtener_semaforo(datos->id_pedido);
	sem_wait(sincronizador);
	log_info(logger, "paso el wait");
	bool op_ok = cliente_enviar_mensaje(cliente_comanda, FINALIZAR_PEDIDO, datos);
	log_info(logger, "mando mensaje a comanda %d", op_ok);
	return respuesta_crear(FINALIZAR_PEDIDO_RESPUESTA, (void*) op_ok , false);
}*/

static t_respuesta* operacion_terminar()
{
	agregar_interrupcion(TERMINAR_APP, NULL);
	return respuesta_crear(TERMINAR, NULL, false);
}

void cargar_interfaz()
{
	servidor_agregar_operacion(servidor, TERMINAR, &operacion_terminar);
	servidor_agregar_operacion(servidor, HANDSHAKE_CLIENTE, &handshake_cliente);
	servidor_agregar_operacion(servidor, CONEXION_CLIENTE, &conexion_cliente);
	servidor_agregar_operacion(servidor, HANDSHAKE_RESTO_APP, &handshake_restaurante_app);

	servidor_agregar_operacion(servidor, CONSULTAR_RESTAURANTES, &consultar_restaurantes);
	servidor_agregar_operacion(servidor, SELECCIONAR_RESTAURANTE, &seleccionar_restaurante);
	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
	servidor_agregar_operacion(servidor, CREAR_PEDIDO, &crear_pedido);
	servidor_agregar_operacion(servidor, AGREGAR_PLATO, &agregar_plato);
	servidor_agregar_operacion(servidor, PLATO_LISTO, &plato_listo);
	servidor_agregar_operacion(servidor, CONFIRMAR_PEDIDO, &confirmar_pedido);
	servidor_agregar_operacion(servidor, CONSULTAR_PEDIDO, &consultar_pedido);
	//servidor_agregar_operacion(servidor, FINALIZAR_PEDIDO, &finalizar_pedido);
}

