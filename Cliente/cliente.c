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

//Obtener Restaurante
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
static void desconectar() {	hay_que_leer= false; }
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

//========== OPERACIONES ==========//
static void operacion_consultar_restaurantes(t_list* restaurantes)
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
	{
		restaurante_seleccionado = NULL;
		consola_log(consola, "Error al seleccionar el restaurante.");
	}
}

static void operacion_consultar_platos(t_list* platos)
{
	consola_log(consola, "Platos: ");

	void loggear_restaurante(void* plato) { consola_log(consola, plato); }
	list_iterate(platos, &loggear_restaurante);
}

static void operacion_terminar_servidor() { consola_log(consola, "El servidor se cerro correctamente."); }

//========== CLIENTE ==========//
static void inicializar()
{
	config = config_create("cliente.config");
	diccionario_serializaciones_inicializar();
	diccionario_deserializaciones_inicializar();
	diccionario_destrucciones_inicializar();
	hay_que_leer = true;

	//=== DATOS TEMPORALES ===//
	id_pedido = 0;
	restaurante_seleccionado = NULL;

	//=== CLIENTE RED ===//
	cliente = cliente_crear(config_get_string_value(config, "IP_APP"), config_get_string_value(config, "PUERTO_APP"));
	cliente_agregar_operacion(cliente, CONSULTAR_RESTAURANTES, &operacion_consultar_restaurantes);
	cliente_agregar_operacion(cliente, SELECCIONAR_RESTAURANTE_RESPUESTA, &operacion_seleccionar_restaurante);
	cliente_agregar_operacion(cliente, CONSULTAR_PLATOS_RESPUESTA, &operacion_consultar_platos);
	cliente_agregar_operacion(cliente, TERMINAR, &operacion_terminar_servidor);

	//=== CONSOLA ===//
	consola = consola_crear("cliente.log", "Cliente"); //config_get_string_value(config, "ARCHIVO_LOG")
	consola_agregar_comando(consola, "desconectar", &desconectar);
	consola_agregar_comando(consola, "terminar", &terminar);
	consola_agregar_comando(consola, "consultar restaurantes", &consultar_restaurantes);
	consola_agregar_comando(consola, "seleccionar restaurante", &seleccionar_restaurante);
	consola_agregar_comando(consola, "consultar platos", &consultar_platos);
}

static void terminar_programa()
{
	config_destroy(config);
	consola_destruir(consola);
	cliente_destruir(cliente);
	diccionario_serializaciones_destruir();
	diccionario_deserializaciones_destruir();
	diccionario_destrucciones_destruir();
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

