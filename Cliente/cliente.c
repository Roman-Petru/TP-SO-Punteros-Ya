#include "cliente.h"
#include "../Utils/consola.h"
#include "../Utils/protocolo.h"
#include "../Utils/cliente_red.h"
#include "../Utils/serializacion.h"
#include <commons/config.h>

t_cliente_red* cliente;
t_consola* consola;
bool hay_que_leer;
int id_pedido;

//Obtener Restaurante
//Consultar Platos
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

//========== COMANDOS ==========//
static void consultar_restaurantes() { cliente_enviar_mensaje(cliente, CONSULTAR_RESTAURANTES, NULL); }

static void seleccionar_restaurante()
{
	char* restaurante = consola_leer("Ingrese el nombre del restaurante: ");
	cliente_enviar_mensaje(cliente, SELECCIONAR_RESTAURANTE, restaurante);
}

/*static void consultar_platos()
{
	char* restaurante = consola_leer("Ingrese el nombre del restaurante: ");
	cliente_enviar_mensaje(cliente, CONSULTAR_PLATOS, restaurante);
}*/

//========== OPERACIONES ==========//
static void operacion_consultar_restaurtantes(t_list* restaurantes)
{
	consola_log(consola, "Restaurantes: ");

	void loggear_restaurante(void* restaurante) { consola_log(consola, restaurante); }
	list_iterate(restaurantes, &loggear_restaurante);
}

static void operacion_seleccionar_restaurante(bool operacion_ok)
{
	if(operacion_ok)
		consola_log(consola, "Restaurante seleccionado correctamente.");
	else
		consola_log(consola, "Error al seleccionar el restaurante.");
}

/*static void operacion_consultar_platos(t_list* platos)
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

//========== CLIENTE ==========//
static void inicializar()
{
	t_config* config = config_create("cliente.config");
	diccionario_serializaciones_inicializar();
	diccionario_deserializaciones_inicializar();
	hay_que_leer = true;
	id_pedido = 0;

	//=== CLIENTE RED ===//
	cliente = cliente_crear(config_get_string_value(config, "IP_APP"), config_get_string_value(config, "PUERTO_APP"));
	cliente_agregar_operacion(cliente, CONSULTAR_RESTAURANTES, &operacion_consultar_restaurtantes);
	cliente_agregar_operacion(cliente, SELECCIONAR_RESTAURANTE, &operacion_seleccionar_restaurante);

	//=== CONSOLA ===//
	consola = consola_crear("cliente.log", "Cliente"); //config_get_string_value(config, "ARCHIVO_LOG")
	consola_agregar_comando(consola, "consultar restaurantes", &consultar_restaurantes);
	consola_agregar_comando(consola, "seleccionar restaurante", &seleccionar_restaurante);
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
