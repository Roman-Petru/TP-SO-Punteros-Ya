#include "cliente.h"
#include "../Utils/consola.h"
#include "../Utils/protocolo.h"
#include "../Utils/cliente_red.h"
#include "../Utils/serializacion.h"
#include <commons/config.h>

t_cliente_red* cliente;
t_consola* consola;
t_config* config;
bool hay_que_leer;

//DATOS TEMPORALES
int id_pedido;
char* restaurante_seleccionado;



static void terminar_programa()
{
	config_destroy(config);
	consola_destruir(consola);
	cliente_destruir(cliente);
	serializacion_finalizar();
}



int main()
{
	inicializar();
	while(hay_que_leer)
		consola_leer_comando(consola, "Cliente: ");
	terminar_programa();
}



