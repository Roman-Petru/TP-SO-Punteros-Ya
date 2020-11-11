#include "interfaz.h"

//Consultar Pedido
//Obtener Pedido
//Finalizar Pedido
//Obtener Receta

//========== VALIDACIONES ==========//
static bool validar_restaurante()
{
	bool invalido = restaurante_seleccionado == NULL;

	if(invalido)
		consola_log(consola, "No se selecciono ningun Restaurante. ");

	return invalido;
}

static bool validar_pedido()
{
	bool invalido = id_pedido<0;

	if(invalido)
		consola_log(consola, "No hay pedido en marcha.");
	return invalido;
}

//========== COMANDOS ==========//
static void desconectar() {	hay_que_leer = false; }

static void terminar()
{
	hay_que_leer= false;
	cliente_enviar_mensaje(cliente, TERMINAR, NULL);
	consola_log(consola, "El servidor se cerro correctamente.");
}

//========== INTERFAZ ==========//
void handshake()
{
	t_modulo modulo = (t_modulo) cliente_enviar_mensaje(cliente, HANDSHAKE_CLIENTE, NULL);

	bool op_ok = true;



	if(modulo == APP)
	{
		t_datos_cliente* datos = crear_datos_cliente(config_get_string_value(config, "ID_CLIENTE"), posicion_crear(config_get_int_value(config, "POSICION_X"), config_get_int_value(config, "POSICION_Y")));
		op_ok = cliente_enviar_mensaje(cliente, CONEXION_CLIENTE, datos);
	}


	if(modulo == MODULO_ERROR || !op_ok)
		consola_log(consola, "Error al realizar el handshake.");
}

static void consultar_restaurantes()
{
	t_list* restaurantes = cliente_enviar_mensaje(cliente, CONSULTAR_RESTAURANTES, NULL);

	consola_log(consola, "Restaurantes: ");

	void loggear_restaurante(void* restaurante) { consola_log(consola, restaurante); }
	list_iterate(restaurantes, &loggear_restaurante);

	destruir_lista_string(restaurantes);
}

static void seleccionar_restaurante()
{
	char* restaurante = consola_leer("Ingrese el nombre del restaurante: ");
	if(restaurante_seleccionado!=NULL)
		free(restaurante_seleccionado);
	restaurante_seleccionado = restaurante;

	bool operacion_ok = cliente_enviar_mensaje(cliente, SELECCIONAR_RESTAURANTE, crear_datos_seleccion_restaurante(config_get_string_value(config, "ID_CLIENTE"), restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}

static void obtener_restaurante()
{
	if(validar_restaurante())
		return;

	//void* restaurante = cliente_enviar_mensaje(cliente, "SINDICATO", OBTENER_RESTAURANTE, restaurante_seleccionado);
	cliente_enviar_mensaje(cliente, OBTENER_RESTAURANTE, restaurante_seleccionado);

	consola_log(consola, "Datos Restaurante: ");
}

static void consultar_platos()
{
	if(validar_restaurante())
		return;

	t_list* platos = cliente_enviar_mensaje(cliente, CONSULTAR_PLATOS, restaurante_seleccionado);

	consola_log(consola, "Platos: ");

	void loggear_restaurante(void* plato) { consola_log(consola, plato); }
	list_iterate(platos, &loggear_restaurante);
	destruir_lista_string(platos);
}

static void crear_pedido()
{
	if(validar_restaurante())
		return;

	id_pedido = (int) cliente_enviar_mensaje(cliente, CREAR_PEDIDO, config_get_string_value(config, "ID_CLIENTE"));

	consola_if(consola, id_pedido > 0);
}

static void guardar_pedido()
{
	if(validar_restaurante() && validar_pedido())
		return;

	t_datos_pedido datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;

	bool operacion_ok = cliente_enviar_mensaje(cliente, CREAR_PEDIDO, &datos);
	consola_if(consola, operacion_ok);
}

static void agregar_plato()
{
	if(validar_pedido())
		return;

	t_agregar_plato* datos = crear_datos_agregar_plato(id_pedido, consola_leer("Ingrese el nombre del plato: "));
	bool operacion_ok = cliente_enviar_mensaje(cliente, AGREGAR_PLATO, datos);
	consola_if(consola, operacion_ok);
}

static void guardar_plato()
{
	if(validar_pedido() && validar_restaurante())
		return;

	t_guardar_plato datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;
	datos.comida = consola_leer("Ingrese el nombre de la comida: ");
	datos.restaurante = consola_leer("Ingrese la cantidad: ");

	bool operacion_ok = cliente_enviar_mensaje(cliente, GUARDAR_PLATO, &datos);
	consola_if(consola, operacion_ok);

	free(datos.comida);
	free(datos.restaurante);
}

static void confirmar_pedido()
{
	if(validar_restaurante() && validar_pedido())
		return;

	bool operacion_ok = cliente_enviar_mensaje(cliente, CONFIRMAR_PEDIDO, crear_datos_pedido(id_pedido, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}

static void plato_listo()
{

	if(validar_pedido() && validar_restaurante())
		return;

	bool operacion_ok = cliente_enviar_mensaje(cliente, PLATO_LISTO, crear_datos_plato_listo(id_pedido, consola_leer("Ingrese el nombre de la comida: "), restaurante_seleccionado));
	consola_if(consola, operacion_ok);
	//QUIZAS BORRAR COMIDA QUE SE LEE X CONSOLA
}

static void consultar_pedido()
{
	if(validar_pedido())
		return;

	t_consultar_pedido* pedido = cliente_enviar_mensaje(cliente, CONSULTAR_PEDIDO, (void*) id_pedido);

	char mensaje[80];
	sprintf(mensaje, "Pedido: %d", id_pedido);
	consola_log(consola, mensaje);

	sprintf(mensaje, "Estado: %s", pedido->estado ? "Listo" : "En Produccion");
	consola_log(consola, mensaje);

	sprintf(mensaje, "Plaso:");
	consola_log(consola, mensaje);

	void logguear_plato(void* plato) { consola_log(consola, plato); };
	list_iterate(pedido->platos, &logguear_plato);

	destruir_consultar_pedido(pedido);
}

static void terminar_pedido()
{
	if(validar_pedido() && validar_restaurante())
		return;
	/*
	t_datos_pedido datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;
	 */
	bool operacion_ok = cliente_enviar_mensaje(cliente, TERMINAR_PEDIDO, crear_datos_pedido(id_pedido, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}

void cargar_interfaz()
{
	consola_agregar_comando(consola, "desconectar", &desconectar);
	consola_agregar_comando(consola, "terminar", &terminar);

	//=== INTERFAZ ===//
	consola_agregar_comando(consola, "consultar restaurantes", &consultar_restaurantes);
	consola_agregar_comando(consola, "seleccionar restaurante", &seleccionar_restaurante);
	consola_agregar_comando(consola, "obtener restaurante", &obtener_restaurante);
	consola_agregar_comando(consola, "consultar platos", &consultar_platos);
	consola_agregar_comando(consola, "crear pedido", &crear_pedido);
	consola_agregar_comando(consola, "guardar pedido", &guardar_pedido);
	consola_agregar_comando(consola, "agregar plato", &agregar_plato);
	consola_agregar_comando(consola, "guardar plato", &guardar_plato);
	consola_agregar_comando(consola, "confirmar pedido", &confirmar_pedido);
	consola_agregar_comando(consola, "plato listo", &plato_listo);
	consola_agregar_comando(consola, "terminar pedido", &terminar_pedido);
	consola_agregar_comando(consola, "consultar pedido", &consultar_pedido);
}

