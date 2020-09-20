#include "cliente.h"
#include "../Utils/consola.h"
#include "../Utils/cliente_red.h"
//#include "../Utils/consola.h"


t_cliente_red* cliente;
t_consola* consola;
bool hay_que_leer;

//========== COMANDOS ==========//
//Definir comandos
static void cliente_hola()
{
	cliente_enviar_mensaje(cliente, CONSOLA, "hola");
}

//========== CLIENTE ==========//
static void inicializar()
{
	diccionario_serializaciones_inicializar();
	hay_que_leer = true;
	cliente = cliente_crear("127.0.0.1", "4444");

	//=== CONSOLA ===//
	consola = consola_crear("cliente.log", "Cliente");

	//Agregar comandos a consola
	consola_agregar_comando(consola, "hola", &cliente_hola);
}

static void terminar_programa()
{
	consola_destruir(consola);
	cliente_destruir(cliente);
	diccionario_serializaciones_destruir();
}


int main()
{
	inicializar();
	while(hay_que_leer)
		consola_leer_comando(consola, "Cliente: ");
	terminar_programa();
}
