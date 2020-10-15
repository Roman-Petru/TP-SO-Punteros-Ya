#include "operaciones.h"

//Crear Pedido
//Guardar Pedido
//Añadir Plato
//Guardar Plato
//Confirmar Pedido
//Plato Listo
//Consultar Pedido
//Obtener Pedido
//Finalizar Pedido
//Terminar Pedido
//Obtener Receta

static void terminar_servidor() { consola_log(consola, "El servidor se cerro correctamente."); }

//========== INTERFAZ ==========//
static void consultar_restaurantes(t_list* restaurantes)
{
	consola_log(consola, "Restaurantes: ");

	void loggear_restaurante(void* restaurante) { consola_log(consola, restaurante); }
	list_iterate(restaurantes, &loggear_restaurante);
}

static void seleccionar_restaurante(bool operacion_ok)
{
	if(operacion_ok)
		consola_log(consola, "Restaurante seleccionado correctamente.");
	else
	{
		restaurante_seleccionado = NULL;
		consola_log(consola, "ERROR: Restaurante Invalido.");
	}
}

static void obtener_restaurante(void* restaurantes) // void -> t_restaurante
{
	consola_log(consola, "Datos Restaurante: ");
}

static void consultar_platos(t_list* platos)
{
	consola_log(consola, "Platos: ");

	void loggear_restaurante(void* plato) { consola_log(consola, plato); }
	list_iterate(platos, &loggear_restaurante);
}

static void crear_pedido(int id_nuevo_pedido)
{
	if(id_nuevo_pedido == -1)
	{
		consola_log(consola, "ERROR: El pedido no pudo ser creado.");
		return;
	}
	id_pedido = id_nuevo_pedido;
}

//========== CLIENTE RED ==========//
void agregar_operaciones()
{
	cliente_agregar_operacion(cliente, CONSULTAR_RESTAURANTES_RESPUESTA, &consultar_restaurantes);
	cliente_agregar_operacion(cliente, SELECCIONAR_RESTAURANTE_RESPUESTA, &seleccionar_restaurante);
	cliente_agregar_operacion(cliente, OBTENER_RESTAURANTE_RESPUESTA, &obtener_restaurante);
	cliente_agregar_operacion(cliente, CONSULTAR_PLATOS_RESPUESTA, &consultar_platos);
	cliente_agregar_operacion(cliente, CREAR_PEDIDO_RESPUESTA, &crear_pedido);

	cliente_agregar_operacion(cliente, TERMINAR, &terminar_servidor);
}
