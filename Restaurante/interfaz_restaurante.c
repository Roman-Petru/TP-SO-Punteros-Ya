
#include "interfaz_restaurante.h"
#include "restaurante.h"

t_servidor_red* servidor_restaurant;
//Va a recibir los siguientes mensajes:

	//Consultar Platos: La App consulta los platos al Restaurante y esta es la respuesta

static t_respuesta* consultar_platos(char* restaurante)
{
	log_info(logger_app, "Me consultaron platos %s.", restaurante);
	t_list* platos = list_create();

	//TODO App: consultar_platos
	//if(esta_conectado(restaurante))
	//	consultar platos a restaurante y cargar en t_list* platos
	//else
		platos_default(platos);
	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, platos);
}

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


void inicializar_servidor()
{
	serializacion_inicializar();
	servidor_restaurant = servidor_crear("127.0.0.1", config_get_string_value(config_restaurante, "PUERTO_ESCUCHA"));
	servidor_agregar_operacion(servidor_restaurant, CONSULTAR_PLATOS_RESPUESTA, &consultar_platos);
	consola_agregar_comando(consola, "consultar platos", &operacion_consultar_platos);

	log_info(logger_restaurante, "Servidor listo para recibir al restaurante");
}
