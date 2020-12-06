#include "interfaz_restaurante.h"
#include "restaurante.h"

void realizar_handshake_con_app()
{

	void* operacion_ok = cliente_enviar_mensaje(cliente_app, HANDSHAKE_RESTO_APP, crear_datos_handshake_restaurante_app(nombre_restaurante, config_get_string_value(config_resto, "IP_SERVIDOR"), config_get_string_value(config_resto, "PUERTO_ESCUCHA"), posicion));

	if (operacion_ok == NULL)
		{log_info(logger_resto, "Abierto el restaurante %s, activado modo funcionamiento sin app", nombre_restaurante);
		app_activada = false;
		return;}

	log_info(logger_resto, "Abierto el restaurante %s, se hizo correctamente el handshake con la app", nombre_restaurante);
	app_activada = true;
}

static t_respuesta* handshake_cliente()
{
	log_info(logger_resto, "Un cliente realizo un handshake");
	return respuesta_crear(HANDSHAKE_CLIENTE_RESPUESTA, (void*) RESTAURANTE, false);
}


static t_respuesta* consultar_platos(char* restaurante)
{
	log_info(logger_resto, "Me consultaron los platos");

	t_list* platos = cliente_enviar_mensaje(cliente_sind, CONSULTAR_PLATOS, nombre_restaurante);

	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, platos, true);
}

static t_respuesta* crear_pedido()
{
	log_info(logger_resto, "Me pidieron crear pedido.");

	int id_nuevo_pedido = indice_id_pedido;
	indice_id_pedido ++;

	bool operacion_ok = cliente_enviar_mensaje(cliente_sind, GUARDAR_PEDIDO, crear_datos_pedido(id_nuevo_pedido, nombre_restaurante));

	if (operacion_ok)
		log_info(logger_resto, "Se mando correctamente el mensaje Guardar Pedido al sindicato");
	else log_info(logger_resto, "No se mando correctamente el mensaje Guardar Pedido al sindicato");

	return respuesta_crear(CREAR_PEDIDO_RESPUESTA, (void*) id_nuevo_pedido, false);
}

static t_respuesta* agregar_plato(t_agregar_plato* datos_para_agregar_plato)
{
	if (!existe_plato(datos_para_agregar_plato->plato))
		{	log_info(logger_resto, "No se agrego plato ya que no lo hace el restaurante");
		return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) false, false);	}

	bool operacion_ok = cliente_enviar_mensaje(cliente_sind, GUARDAR_PLATO, crear_datos_guardar_plato(datos_para_agregar_plato->id_pedido, 1, datos_para_agregar_plato->plato , nombre_restaurante));

		if (operacion_ok)
			return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) true, false);
		else
			return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) false, false);
}

static t_respuesta* confirmar_pedido(t_datos_pedido* datos_para_confirmar)
{
	t_datos_estado_pedido* datos_pedido = cliente_enviar_mensaje(cliente_sind, OBTENER_PEDIDO, crear_datos_pedido(datos_para_confirmar->id_pedido, nombre_restaurante));

	bool op_ok = obtener_recetas(datos_pedido->platos);
	if (!op_ok)
		{log_info(logger_resto, "No se pudo confirmar el pedido ya que no se encontro alguna receta");
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);}


	bool conf_ok = cliente_enviar_mensaje(cliente_sind, CONFIRMAR_PEDIDO, crear_datos_pedido(datos_para_confirmar->id_pedido, nombre_restaurante));
	if (!conf_ok)
		{log_info(logger_resto, "No se pudo confirmar el pedido ya que el sindicato no lo pudo confirmar");
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);}



	void empezar_pedido (void* en_lista) {
		t_datos_estado_comida* plato = en_lista;
		for (int i=0; i<plato->cant_total;i++)
			{t_para_nuevo_plato* nuevo_plato = malloc(sizeof(t_para_nuevo_plato));
			nuevo_plato->nombre_plato = plato->comida;
			nuevo_plato->id_pedido = datos_para_confirmar->id_pedido;
			agregar_interrupcion(NUEVO_PLATO, nuevo_plato);}}


	list_iterate(datos_pedido->platos, &empezar_pedido);

	agendar_pedido(datos_para_confirmar->id_pedido, datos_pedido);
	return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) true, false);
}

static t_respuesta* consultar_pedido(t_datos_pedido* datos_para_confirmar)
{

	t_datos_estado_pedido* datos_pedido = cliente_enviar_mensaje(cliente_sind, OBTENER_PEDIDO, crear_datos_pedido(datos_para_confirmar->id_pedido, nombre_restaurante));

	return respuesta_crear(CONSULTAR_PEDIDO_RESPUESTA, crear_datos_consultar_pedido(nombre_restaurante, datos_pedido->estado, datos_pedido->platos), false);
}

/*FINALIZAR PEDIDO*/
static t_respuesta* finalizar_pedido(t_datos_pedido* datos)
{
	bool buscar_pedido (void* estado_platos)
	{t_platos_listos* platos_listos = estado_platos;
	return platos_listos->id_pedido == datos->id_pedido;}

	pthread_mutex_lock(&mutex_pedidos);
	t_platos_listos* platos_listos = list_find(lista_pedidos, &buscar_pedido);
	pthread_mutex_unlock(&mutex_pedidos);
	sem_wait(platos_listos->sincronizador);

	return respuesta_crear(FINALIZAR_PEDIDO_RESPUESTA, (void*) true , false);
}

void cargar_interfaz()
{
	serializacion_inicializar();

	servidor_agregar_operacion(servidor, HANDSHAKE_CLIENTE, &handshake_cliente);
	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
	servidor_agregar_operacion(servidor, CREAR_PEDIDO, &crear_pedido);
	servidor_agregar_operacion(servidor, AGREGAR_PLATO, &agregar_plato);
	servidor_agregar_operacion(servidor, CONFIRMAR_PEDIDO, &confirmar_pedido);
	servidor_agregar_operacion(servidor, CONSULTAR_PEDIDO, &consultar_pedido);
	servidor_agregar_operacion(servidor, FINALIZAR_PEDIDO, &finalizar_pedido);

}

