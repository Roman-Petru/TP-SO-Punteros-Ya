#include "interfaz.h"

//========== COMANDOS ==========//
static void desconectar() {	hay_que_leer = false; }
static void terminar()
{
	hay_que_leer= false;
	cliente_enviar_mensaje(cliente, TERMINAR, NULL);
}

static void consultar_restaurantes() { cliente_enviar_mensaje(cliente, CONSULTAR_RESTAURANTES, NULL); }

static void seleccionar_restaurante()
{
	char* restaurante = consola_leer("Ingrese el nombre del restaurante: ");
	restaurante_seleccionado = restaurante;
	cliente_enviar_mensaje(cliente, SELECCIONAR_RESTAURANTE, restaurante);
}

static void consultar_platos()
{
	if(restaurante_seleccionado == NULL)
	{
		consola_log(consola, "No se selecciono ningun Restaurante. ");
		return;
	}
	cliente_enviar_mensaje(cliente, CONSULTAR_PLATOS, restaurante_seleccionado);
}

void agregar_comandos()
{
	consola_agregar_comando(consola, "desconectar", &desconectar);
	consola_agregar_comando(consola, "terminar", &terminar);
	consola_agregar_comando(consola, "consultar restaurantes", &consultar_restaurantes);
	consola_agregar_comando(consola, "seleccionar restaurante", &seleccionar_restaurante);
	consola_agregar_comando(consola, "consultar platos", &consultar_platos);
}

