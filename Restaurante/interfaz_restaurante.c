
#include "interfaz_restaurante.h"
#include "restaurante.h"

t_servidor_red* servidor_restaurant;
//Va a recibir los siguientes mensajes:


	//Consultar Platos: El Restaurante le consulta los platos al Sindicato

static void operacion_consultar_platos(t_list* platos)
{
	consola_log(consola, "Platos: ");

	void loggear_sindicato(void* plato) { consola_log(consola, plato); }
	list_iterate(platos, &loggear_sindicato);
}

	//Crear Pedido

	//Añadir Plato

	//Confirmar Pedido

	//Consultar Pedido


void inicializar_restaurante()
{
	serializacion_inicializar();
	servidor_restaurant = servidor_crear("127.0.0.1", config_get_string_value(config_resto, "PUERTO_ESCUCHA"));
	consola_agregar_comando(consola, "consultar platos", &operacion_consultar_platos); //TODO ver si esto es necesario

	log_info(logger_resto, "Servidor listo para recibir al restaurante");
}
