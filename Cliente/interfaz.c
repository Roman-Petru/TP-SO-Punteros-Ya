#include "comandos.h"

//========== VALIDACIONES ==========//
static char* validar_modulo(char* modulo)
{
	if(strcmp(modulo, "APP")==0 || strcmp(modulo, "app")==0)
		return "APP";
	else if(strcmp(modulo, "RESTAURANTE")==0 || strcmp(modulo, "restaurante")==0)
		return "RESTAURANTE";
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
}

void seleccionar_modulo()
{
	servidor = validar_modulo(consola_leer("Ingrese el nombre del modulo: "));
	if(servidor == NULL)
		consola_log(consola, "No se reconoce el modulo.");
	else
		consola_log(consola, "Modulo seleccionado correctamente.");
}

//========== INTERFAZ ==========//
static void consultar_restaurantes() { cliente_enviar_mensaje(cliente, "APP", CONSULTAR_RESTAURANTES, NULL); }

static void seleccionar_restaurante()
{
	char* restaurante = consola_leer("Ingrese el nombre del restaurante: ");
	restaurante_seleccionado = restaurante;
	cliente_enviar_mensaje(cliente, "APP", SELECCIONAR_RESTAURANTE, restaurante);
}

static void obtener_restaurante()
{
	if(validar_restaurante())
		return;

	cliente_enviar_mensaje(cliente, "SINDICATO", OBTENER_RESTAURANTE, restaurante_seleccionado);
}

static void consultar_platos()
{
	char* modulos[] = { "APP", "RESTAURANTE", "SINDICATO" };
	if(validar_restaurante() && validar_servidor(modulos, 3, true))
		return;

	cliente_enviar_mensaje(cliente, servidor, CONSULTAR_PLATOS, restaurante_seleccionado);
}

static void crear_pedido()
{
	char* modulos[] = { "APP", "RESTAURANTE" };
	if(validar_servidor(modulos, 2, true))
		return;

	cliente_enviar_mensaje(cliente, servidor, CREAR_PEDIDO, NULL);
}

static void guardar_pedido()
{
	if(validar_restaurante() && validar_pedido())
		return;

	t_datos_pedido datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;

	cliente_enviar_mensaje(cliente, servidor, CREAR_PEDIDO, &datos);
}

static void agregar_plato()
{
	if(validar_pedido())
		return;

	t_agregar_plato datos;
	datos.plato = consola_leer("Ingrese el nombre del plato: ");;
	datos.id_pedido = id_pedido;

	cliente_enviar_mensaje(cliente, servidor, AGREGAR_PLATO, &datos);
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

	cliente_enviar_mensaje(cliente, servidor, GUARDAR_PLATO, &datos);
}

static void confirmar_pedido() //POLIMORFISMO?
{
	if(validar_restaurante() && validar_pedido())
			return;

	t_datos_pedido datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;

	cliente_enviar_mensaje(cliente, servidor, CONFIRMAR_PEDIDO, &datos);
}

static void plato_listo()
{
	char* modulos[] = { "APP", "SINDICATO", "COMANDA" };
	if(validar_servidor(modulos, 3, true) && validar_pedido() && validar_restaurante())
		return;

	t_plato_listo datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;
	datos.comida = consola_leer("Ingrese el nombre de la comida: ");

	cliente_enviar_mensaje(cliente, servidor, PLATO_LISTO, &datos);
}

static void terminar_pedido()
{
	if(validar_pedido() && validar_restaurante())
		return;

	t_datos_pedido datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;

	cliente_enviar_mensaje(cliente, "SINDICATO", TERMINAR_PEDIDO, &datos);
}

void agregar_comandos()
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

}

