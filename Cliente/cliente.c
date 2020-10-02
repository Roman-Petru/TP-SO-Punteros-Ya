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

/*static void operacion_consultar_PEDIDO(t_list* platos)
{
	consola_log(consola, "Platos: ");

	void loggear_plato(void* plato_void) {
		t_plato* plato = plato_void;
		consola_log(consola, "nombre: %s", plato->nombre);
		consola_log(consola, "cant_total: %s", plato->cant_total);
		consola_log(consola, "cant_lista: %s", plato->cant_lista);
	}

	list_iterate(restaurantes, &loggear_plato);
}*/

