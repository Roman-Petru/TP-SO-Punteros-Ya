#include "interfaz.h"

t_modulo modulo;

//========== VALIDACIONES ==========//
static bool validar_restaurante()
{
	bool invalido = restaurante_seleccionado == NULL;

	if(invalido)
		consola_log(consola, "No se selecciono ningun Restaurante. ");

	return invalido;
}

static bool validar_pedido()
{
	bool invalido = id_pedido<0;

	if(invalido)
		consola_log(consola, "No hay pedido en marcha.");

	return invalido;
}

//========== COMANDOS ==========//
static void desconectar() {	hay_que_leer = false; }

static void terminar()
{
	hay_que_leer= false;
	cliente_enviar_mensaje(cliente, TERMINAR, NULL);
	consola_log(consola, "El servidor se cerro correctamente.");
}

//========== INTERFAZ ==========//
void handshake()
{
	modulo = (t_modulo) cliente_enviar_mensaje(cliente, HANDSHAKE_CLIENTE, NULL);
	bool op_ok = true;

	if(modulo == APP)
	{
		t_datos_cliente* datos = crear_datos_cliente(config_get_string_value(config, "ID_CLIENTE"), posicion_crear(config_get_int_value(config, "POSICION_X"), config_get_int_value(config, "POSICION_Y")));
		op_ok = cliente_enviar_mensaje(cliente, CONEXION_CLIENTE, datos);
	}

	if(modulo == MODULO_ERROR || !op_ok)
		consola_log(consola, "Error al realizar el handshake.");
}

static void consultar_restaurantes()
{
	t_list* restaurantes = cliente_enviar_mensaje(cliente, CONSULTAR_RESTAURANTES, NULL);

	consola_log(consola, "Restaurantes: ");

	void loggear_restaurante(void* restaurante) { consola_log(consola, restaurante); }
	list_iterate(restaurantes, &loggear_restaurante);

	destruir_lista_string(restaurantes);
}

static void seleccionar_restaurante()
{
	char* restaurante = consola_leer("Ingrese el nombre del restaurante: ");
	if(restaurante_seleccionado!=NULL)
		free(restaurante_seleccionado);
	restaurante_seleccionado = restaurante;

	bool operacion_ok = cliente_enviar_mensaje(cliente, SELECCIONAR_RESTAURANTE, crear_datos_seleccion_restaurante(config_get_string_value(config, "ID_CLIENTE"), restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}


static void consultar_platos()
{


	if (modulo == SINDICATO)
		restaurante_seleccionado = consola_leer("Ingrese el nombre del restaurante: ");
	if (modulo == RESTAURANTE)
		restaurante_seleccionado = "no importa";
	if(validar_restaurante())
		return;

	t_list* platos = cliente_enviar_mensaje(cliente, CONSULTAR_PLATOS, restaurante_seleccionado);

	consola_log(consola, "Platos: ");

	void loggear_restaurante(void* plato) { consola_log(consola, plato); }
	list_iterate(platos, &loggear_restaurante);
	destruir_lista_string(platos);
}

static void crear_pedido()
{

	if (modulo == RESTAURANTE)
		restaurante_seleccionado = "no importa";
	else if(validar_restaurante())
		return;


	id_pedido = (int) cliente_enviar_mensaje(cliente, CREAR_PEDIDO, config_get_string_value(config, "ID_CLIENTE"));

	consola_if(consola, id_pedido > 0);
}

static void guardar_pedido()
{
	if(validar_restaurante() && validar_pedido())
		return;

	restaurante_seleccionado = consola_leer("Ingrese el nombre del restaurante: ");
	char* id = consola_leer("Ingrese el id: ");
	id_pedido = strtol(id, NULL, 10);

	t_datos_pedido* datos = crear_datos_pedido(id_pedido, restaurante_seleccionado);

	bool operacion_ok = cliente_enviar_mensaje(cliente, GUARDAR_PEDIDO, datos);
	consola_if(consola, operacion_ok);
}

/*AGREGAR PLATO*/
static void agregar_plato()
{
	if(validar_pedido())
		return;

	t_agregar_plato* datos = crear_datos_agregar_plato(id_pedido, consola_leer("Ingrese el nombre del plato: "));
	bool operacion_ok = cliente_enviar_mensaje(cliente, AGREGAR_PLATO, datos);
	consola_if(consola, operacion_ok);
}

/*GUARDAR PLATO*/
static void guardar_plato()
{
	if(validar_pedido() && validar_restaurante())
		return;

	char* plato = consola_leer("Ingrese el nombre de la comida: ");
	char* cant_s = consola_leer("Ingrese la cantidad: ");
	int cant = strtol(cant_s, NULL, 10);

	t_guardar_plato* datos = crear_datos_guardar_plato(id_pedido,cant ,plato , restaurante_seleccionado);

	bool operacion_ok = cliente_enviar_mensaje(cliente, GUARDAR_PLATO, datos);
	consola_if(consola, operacion_ok);
}

/*OBTENER PEDIDO*/
static void obtener_pedido()
{
	if(validar_pedido() && validar_restaurante())
		return;

	t_datos_pedido* datos = crear_datos_pedido(id_pedido, restaurante_seleccionado);

	t_datos_estado_pedido* datos_pedido = cliente_enviar_mensaje(cliente, OBTENER_PEDIDO, datos);

	char mensaje[40];
	sprintf(mensaje, "El estado del pedido es: %d, y contiene: ", datos_pedido->estado);
	consola_log(consola, mensaje);

	void logear_platos(void* plato)
		{t_datos_estado_comida* plato_a_logear = plato;
		char* mensaje2 = malloc(100);
		sprintf(mensaje2, "Comida: %s, cantidad lista: %d, cantidad total %d", plato_a_logear->comida, plato_a_logear->cant_lista, plato_a_logear->cant_total);
		consola_log(consola, mensaje2);
		free(mensaje2);
		}

	list_iterate(datos_pedido->platos, &logear_platos);
}


static void confirmar_pedido()
{
	if(validar_restaurante() && validar_pedido())
		return;

	bool operacion_ok = cliente_enviar_mensaje(cliente, CONFIRMAR_PEDIDO, crear_datos_pedido(id_pedido, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}

static void plato_listo()
{

	if(validar_pedido() && validar_restaurante())
		return;


	char* plato = consola_leer("Ingrese el nombre del plato listo: ");
	int cant = 1;
	t_guardar_plato* datos = crear_datos_guardar_plato(id_pedido, cant, plato, restaurante_seleccionado);

	bool operacion_ok = cliente_enviar_mensaje(cliente, PLATO_LISTO, datos);
	consola_if(consola, operacion_ok);
}

static void consultar_pedido()
{
	if(validar_pedido())
		return;

	t_consultar_pedido* datos_pedido = cliente_enviar_mensaje(cliente, CONSULTAR_PEDIDO, (void*) id_pedido);

	char mensaje[80];
	sprintf(mensaje, "El estado del pedido es: %d, pertenece al restaurante %s, y contiene: ", datos_pedido->estado, datos_pedido->restaurante);
	consola_log(consola, mensaje);

	void logear_platos(void* plato)
		{t_datos_estado_comida* plato_a_logear = plato;
		char* mensaje2 = malloc(100);
		sprintf(mensaje2, "Comida: %s, cantidad lista: %d, cantidad total %d", plato_a_logear->comida, plato_a_logear->cant_lista, plato_a_logear->cant_total);
		consola_log(consola, mensaje2);
		free(mensaje2);
		}

	list_iterate(datos_pedido->platos, &logear_platos);
}

//========== OBTENER RECETA ==========//

static void obtener_receta()
{

	restaurante_seleccionado = consola_leer("Ingrese el nombre del restaurante: ");

	char* receta = consola_leer("Ingrese el nombre de la receta a obtener: ");

	t_obtener_receta* pasos_receta = cliente_enviar_mensaje(cliente, OBTENER_RECETA, receta);

	consola_log(consola, "Pasos de la receta: ");

	void logear_receta(void* paso)
		{t_paso* paso_a_logear = paso;
		char* mensaje2 = malloc(100);
		sprintf(mensaje2, "Operacion: %s, Tiempo: %d", paso_a_logear->operacion, paso_a_logear->ciclos);
		consola_log(consola, mensaje2);
		free(mensaje2);
		}

	list_iterate(pasos_receta->pasos, &logear_receta);
}



static void obtener_restaurante()
{

	restaurante_seleccionado = consola_leer("Ingrese el nombre del restaurante: ");

	t_obtener_restaurante* datos_resto = cliente_enviar_mensaje(cliente, OBTENER_RESTAURANTE, restaurante_seleccionado);


	if (datos_resto->cantidad_cocineros == 9753)
		{consola_log(consola, "No existe el restaurante");
		return;	}


	consola_log(consola, "Datos del restaurante: ");
	consola_log(consola, "Afinidades: ");
	void logear_afinidades(void* afinidad)
		{char* afi = afinidad;
		char* mensaje = malloc(100);
		sprintf(mensaje, "Afinidad: %s", afi);
		consola_log(consola, mensaje);
		free(mensaje);
		}

	list_iterate(datos_resto->lista_afinidades, &logear_afinidades);



	consola_log(consola, "Lista Precios: ");
	void logear_precios(void* precio)
		{t_precio* precio_str = precio;
		char* mensaje2 = malloc(100);
		sprintf(mensaje2, "Plato: %s, precio: %d", precio_str->nombre_plato, precio_str->precio);
		consola_log(consola, mensaje2);
		free(mensaje2);
		}

	list_iterate(datos_resto->lista_precios, &logear_precios);

	char* mensaje3 = malloc(200);
			sprintf(mensaje3, "Posicion: %d, %d; Cantidad cocineros: %d; Cantidad hornos: %d; Cantidad pedidos: %d", datos_resto->posicion->x, datos_resto->posicion->y, datos_resto->cantidad_cocineros, datos_resto->cantidad_hornos, datos_resto->cantidad_pedidos);
			consola_log(consola, mensaje3);
			free(mensaje3);

}

static void terminar_pedido()
{
	if(validar_pedido() && validar_restaurante())
		return;
	/*
	t_datos_pedido datos;
	datos.restaurante = restaurante_seleccionado;
	datos.id_pedido = id_pedido;
	 */

	bool operacion_ok = cliente_enviar_mensaje(cliente, TERMINAR_PEDIDO, crear_datos_pedido(id_pedido, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}


//========== FINALIZAR PEDIDO ==========//

static void finalizar_pedido()
{
	if(validar_pedido() && validar_restaurante())
		return;

	t_datos_pedido* datos = crear_datos_pedido(id_pedido, restaurante_seleccionado);

	bool operacion_ok = cliente_enviar_mensaje(cliente, FINALIZAR_PEDIDO, datos);
	consola_if(consola, operacion_ok);

}

void cargar_interfaz()
{
	consola_agregar_comando(consola, "desconectar", &desconectar);
	consola_agregar_comando(consola, "terminar", &terminar);

	//=== INTERFAZ ===//
	consola_agregar_comando(consola, "consultar restaurantes", &consultar_restaurantes);
	consola_agregar_comando(consola, "seleccionar restaurante", &seleccionar_restaurante);
	consola_agregar_comando(consola, "obtener restaurante", &obtener_restaurante);
	consola_agregar_comando(consola, "consultar platos", &consultar_platos);
	consola_agregar_comando(consola, "crear pedido", &crear_pedido);
	consola_agregar_comando(consola, "guardar pedido", &guardar_pedido);
	consola_agregar_comando(consola, "obtener pedido", &obtener_pedido);
	consola_agregar_comando(consola, "agregar plato", &agregar_plato);
	consola_agregar_comando(consola, "guardar plato", &guardar_plato);
	consola_agregar_comando(consola, "confirmar pedido", &confirmar_pedido);
	consola_agregar_comando(consola, "plato listo", &plato_listo);
	consola_agregar_comando(consola, "terminar pedido", &terminar_pedido);
	consola_agregar_comando(consola, "finalizar pedido", &finalizar_pedido);
	consola_agregar_comando(consola, "consultar pedido", &consultar_pedido);
	consola_agregar_comando(consola, "obtener receta", &obtener_receta);
}

