#include "interfaz.h"

//Consultar Pedido
//Obtener Pedido
//Finalizar Pedido
//Obtener Receta

//========== VALIDACIONES ==========//
static char* validar_modulo(char* modulo)
{
	if(strcmp(modulo, "APP")==0 || strcmp(modulo, "app")==0)
		return "APP";
	else if(strcmp(modulo, "RESTAURANTE")==0 || strcmp(modulo, "restaurante")==0)
		{restaurante_seleccionado = "restaurante original";
		return "RESTAURANTE";}
	else if(strcmp(modulo, "COMANDA")==0 || strcmp(modulo, "comanda")==0)
		return "COMANDA";
	else if(strcmp(modulo, "SINDICATO")==0 || strcmp(modulo, "sindicato")==0)
		return "SINDICATO";
	else
		return NULL;
}

static bool validar_servidor(char* modulos[], int cant_modulos, bool log)
{
	bool es_valido;

	for(int i=0;i<cant_modulos;i++)
		es_valido = es_valido || strcmp(servidor, modulos[i])==0;

	if(!es_valido && log)
	{
		consola_log(consola, "El comando solo puede mandarse a los modulos: ");
		for(int i=0;i<cant_modulos;i++)
			consola_log(consola, modulos[i]);
	}

	return es_valido;
}

static bool validar_restaurante()
{
	bool es_valido = restaurante_seleccionado == NULL;

	if(es_valido)
		consola_log(consola, "No se selecciono ningun Restaurante. ");

	return es_valido;
}

static bool validar_pedido()
{
	bool es_valido = id_pedido==0;

	if(!es_valido)
		consola_log(consola, "No hay pedido en marcha.");
	return es_valido;
}

//========== COMANDOS ==========//
static void desconectar() {	hay_que_leer = false; }

static void terminar()
{
	hay_que_leer= false;
	cliente_enviar_mensaje(cliente, servidor, TERMINAR, NULL);
	consola_log(consola, "El servidor se cerro correctamente.");
}

void seleccionar_modulo()
{
	char* modulo = consola_leer("Ingrese el nombre del modulo: ");
	servidor = validar_modulo(modulo);
	free(modulo);
	if(servidor == NULL)
		consola_log(consola, "No se reconoce el modulo.");
	else
		consola_log(consola, "Modulo seleccionado correctamente.");
}

//========== INTERFAZ ==========//
void handshake_con_app()
{
	id = (int) cliente_enviar_mensaje(cliente, "APP", CONEXION_CLIENTE, posicion_crear(config_get_int_value(config, "POSICION_X"), config_get_int_value(config, "POSICION_Y")));

	char mensaje[80];
	sprintf(mensaje, "Id Cliente: %d", id);
	consola_log(consola, mensaje);
}

static void consultar_restaurantes()
{
	t_list* restaurantes = cliente_enviar_mensaje(cliente, "APP", CONSULTAR_RESTAURANTES, NULL);

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

	bool operacion_ok = cliente_enviar_mensaje(cliente, "APP", SELECCIONAR_RESTAURANTE, crear_datos_seleccion_restaurante(id, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}

static void obtener_restaurante()
{
	if(validar_restaurante())
		return;

	//void* restaurante = cliente_enviar_mensaje(cliente, "SINDICATO", OBTENER_RESTAURANTE, restaurante_seleccionado);
	cliente_enviar_mensaje(cliente, "SINDICATO", OBTENER_RESTAURANTE, restaurante_seleccionado);

	consola_log(consola, "Datos Restaurante: ");
}

static void consultar_platos()
{
	char* modulos[] = { "APP", "RESTAURANTE", "SINDICATO" };
	if(validar_restaurante() && validar_servidor(modulos, 3, true))
		return;

	t_list* platos = cliente_enviar_mensaje(cliente, servidor, CONSULTAR_PLATOS, restaurante_seleccionado);

	consola_log(consola, "Platos: ");

	void loggear_restaurante(void* plato) { consola_log(consola, plato); }
	list_iterate(platos, &loggear_restaurante);
	destruir_lista_string(platos);
}

static void crear_pedido()
{
	char* modulos[] = { "APP", "RESTAURANTE" };
	if(validar_servidor(modulos, 2, true))
		return;

	int id_nuevo_pedido = (int) cliente_enviar_mensaje(cliente, servidor, CREAR_PEDIDO, (void*) id);

	if(consola_if(consola, id_nuevo_pedido == -1))
		return;
	id_pedido = id_nuevo_pedido;

	char mensaje[80];
	sprintf(mensaje, "Se creo el pedido con id %d", id_pedido);
	consola_log(consola, mensaje);
}

static void guardar_pedido()
{
	char* modulos[] = { "APP", "RESTAURANTE" };
	if(validar_servidor(modulos, 2, true) && validar_restaurante() && validar_pedido())
		return;

	t_datos_pedido datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;

	bool operacion_ok = cliente_enviar_mensaje(cliente, servidor, CREAR_PEDIDO, &datos);
	consola_if(consola, operacion_ok);
}

static void agregar_plato()
{
	if(validar_pedido())
		return;

	t_agregar_plato datos;
	datos.plato = consola_leer("Ingrese el nombre del plato: ");;
	datos.id_pedido = id_pedido;

	bool operacion_ok = cliente_enviar_mensaje(cliente, servidor, AGREGAR_PLATO, &datos);
	consola_if(consola, operacion_ok);
	free(datos.plato);
}

static void guardar_plato()
{
	char* modulos[] = { "SINDICATO", "COMANDA" };
	if(validar_servidor(modulos, 2, true) && validar_pedido() && validar_restaurante())
		return;

	t_guardar_plato datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;
	datos.comida = consola_leer("Ingrese el nombre de la comida: ");
	datos.restaurante = consola_leer("Ingrese la cantidad: ");

	bool operacion_ok = cliente_enviar_mensaje(cliente, servidor, GUARDAR_PLATO, &datos);
	consola_if(consola, operacion_ok);

	free(datos.comida);
	free(datos.restaurante);
}

static void confirmar_pedido()
{
	if(validar_restaurante() && validar_pedido())
		return;

	bool operacion_ok = cliente_enviar_mensaje(cliente, servidor, CONFIRMAR_PEDIDO, crear_datos_pedido(id_pedido, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}

static void plato_listo()
{
	char* modulos[] = { "APP", "SINDICATO", "COMANDA" };
	if(validar_servidor(modulos, 3, true) && validar_pedido() && validar_restaurante())
		return;

	bool operacion_ok = cliente_enviar_mensaje(cliente, servidor, PLATO_LISTO, crear_datos_plato_listo(id_pedido, consola_leer("Ingrese el nombre de la comida: "), restaurante_seleccionado));
	consola_if(consola, operacion_ok);
	//QUIZAS BORRAR COMIDA QUE SE LEE X CONSOLA
}

static void consultar_pedido()
{
	char* modulos[] = { "APP", "RESTAURANTE" };
	if(validar_servidor(modulos, 3, true) && validar_pedido())
		return;

	t_consultar_pedido* pedido = cliente_enviar_mensaje(cliente, servidor, CONSULTAR_PEDIDO, (void*) id_pedido);

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
	bool operacion_ok = cliente_enviar_mensaje(cliente, "SINDICATO", TERMINAR_PEDIDO, crear_datos_pedido(id_pedido, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}

void cargar_interfaz()
{
	consola_agregar_comando(consola, "desconectar", &desconectar);
	consola_agregar_comando(consola, "terminar", &terminar);
	consola_agregar_comando(consola, "seleccionar_modulo", &seleccionar_modulo);

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

