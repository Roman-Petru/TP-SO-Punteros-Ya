#include "comandos.h"

static void desconectar() {	hay_que_leer = false; }

static void terminar()
{
	hay_que_leer= false;
	cliente_enviar_mensaje(cliente, servidor, TERMINAR, NULL);
}

static void seleccionar_modulo()
{
	char* modulo = consola_leer("Ingrese el modulo: ");
	if(strcmp(modulo, "APP")==0 || strcmp(modulo, "app")==0)
		servidor = "APP";
	else if(strcmp(modulo, "RESTAURANTE")==0 || strcmp(modulo, "restaurante")==0)
		servidor = "RESTAURANTE";
	else if(strcmp(modulo, "COMANDA")==0 || strcmp(modulo, "comanda")==0)
		servidor = "COMANDA";
	else if(strcmp(modulo, "SINDICATO")==0 || strcmp(modulo, "sindicato")==0)
		servidor = "SINDICATO";
	else
		consola_log(consola, "No se reconoce el modulo.");
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
	if(restaurante_seleccionado == NULL)
	{
		consola_log(consola, "No se selecciono ningun Restaurante. ");
		return;
	}
	cliente_enviar_mensaje(cliente, "SINDICATO", OBTENER_RESTAURANTE, restaurante_seleccionado);
}

static void consultar_platos()
{
	if(restaurante_seleccionado == NULL)
	{
		consola_log(consola, "No se selecciono ningun Restaurante. ");
		return;
	}
	if(strcmp(servidor, "COMANDA")==0)
	{
		consola_log(consola, "No se puede consultar platos al modulo Comanda. ");
		return;
	}

	cliente_enviar_mensaje(cliente, servidor, CONSULTAR_PLATOS, restaurante_seleccionado);
}

static void crear_pedido()
{
	if(strcmp(servidor, "APP")!=0 && strcmp(servidor, "RESTAURANTE")!=0 )
	{
		consola_log(consola, "El mensaje solo puede ser enviado al modulo APP o al modulo RESTAURANTE");
		return;
	}
	cliente_enviar_mensaje(cliente, servidor, CREAR_PEDIDO, NULL);
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
}

