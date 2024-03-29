#include "interfaz.h"

t_modulo modulo;
t_dictionary_int* diccionario_estados;

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
		t_datos_cliente* datos = crear_datos_cliente(config_get_string_value(config, "ID_CLIENTE"), posicion_crear(config_get_int_value(config, "POSICION_X"), config_get_int_value(config, "POSICION_Y")), config_get_string_value(config, "IP_SERVIDOR"), config_get_string_value(config, "PUERTO_ESCUCHA"));
		op_ok = cliente_enviar_mensaje(cliente, CONEXION_CLIENTE, datos);
	}

	if(modulo == RESTAURANTE)
	{
		t_datos_cliente* datos = crear_datos_cliente(config_get_string_value(config, "ID_CLIENTE"), posicion_crear(config_get_int_value(config, "POSICION_X"), config_get_int_value(config, "POSICION_Y")), config_get_string_value(config, "IP_SERVIDOR"), config_get_string_value(config, "PUERTO_ESCUCHA"));
		op_ok = cliente_enviar_mensaje(cliente, CONEXION_CLIENTE, datos);
	}

	if(modulo == MODULO_ERROR || !op_ok)
		{consola_log(consola, "Error al realizar el handshake, se intentara reconectar en 5 segundos");
		sleep(5);
		handshake();}
}

static void consultar_restaurantes()
{
	if (modulo != APP)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	t_list* restaurantes = cliente_enviar_mensaje(cliente, CONSULTAR_RESTAURANTES, NULL);

	consola_log(consola, "Restaurantes: ");

	void loggear_restaurante(void* restaurante) { consola_log(consola, restaurante); }
	list_iterate(restaurantes, &loggear_restaurante);

	destruir_lista_string(restaurantes);
}

static void seleccionar_restaurante()
{
	if (modulo != APP)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	char* restaurante = consola_leer("Ingrese el nombre del restaurante: ");
	if(restaurante_seleccionado!=NULL)
		free(restaurante_seleccionado);
	restaurante_seleccionado = restaurante;

	bool operacion_ok = cliente_enviar_mensaje(cliente, SELECCIONAR_RESTAURANTE, crear_datos_seleccion_restaurante( cliente_id, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}


static void consultar_platos()
{
	if (modulo == COMANDA)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	if (modulo == SINDICATO)
		restaurante_seleccionado = consola_leer("Ingrese el nombre del restaurante: ");
	if (modulo == RESTAURANTE)
		restaurante_seleccionado = "no importa";
	if(validar_restaurante())
		return;

	t_list* platos = cliente_enviar_mensaje(cliente, CONSULTAR_PLATOS, restaurante_seleccionado);

	if (list_is_empty(platos))
		{consola_log(consola, "No existe el restaurante seleccionado o no tiene platos");
		list_destroy(platos);
		return;	}

	consola_log(consola, "Platos: ");

	void loggear_restaurante(void* plato) { consola_log(consola, plato); }
	list_iterate(platos, &loggear_restaurante);
	destruir_lista_string(platos);
}

static void crear_pedido()
{
	if (modulo == COMANDA || modulo == SINDICATO)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}


	if (modulo == RESTAURANTE)
		restaurante_seleccionado = "no importa";
	else if(validar_restaurante())
		return;

	id_pedido = (int) cliente_enviar_mensaje(cliente, CREAR_PEDIDO, cliente_id);

	consola_if(consola, id_pedido > 0);
	char* mensaje = malloc(200);
	sprintf(mensaje, "Se creo pedido con id: %d", id_pedido);
	consola_log(consola, mensaje);
	free(mensaje);
}

static void guardar_pedido()
{
	if (modulo == RESTAURANTE || modulo == APP)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	if(validar_restaurante() && validar_pedido())
		return;

	restaurante_seleccionado = consola_leer("Ingrese el nombre del restaurante: ");
	char* id = consola_leer("Ingrese el id del pedido: ");
	id_pedido = strtol(id, NULL, 10);
	free(id);

	t_datos_pedido* datos = crear_datos_pedido(id_pedido, restaurante_seleccionado);

	bool operacion_ok = cliente_enviar_mensaje(cliente, GUARDAR_PEDIDO, datos);
	consola_if(consola, operacion_ok);
}

/*AGREGAR PLATO*/
static void agregar_plato()
{
	if (modulo == COMANDA || modulo == SINDICATO)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	if(validar_pedido())
		return;

	if (modulo == RESTAURANTE)
		{char* id = consola_leer("Ingrese el id del pedido: ");
		id_pedido = strtol(id, NULL, 10);
		free(id);}

	t_agregar_plato* datos = crear_datos_agregar_plato(id_pedido, consola_leer("Ingrese el nombre del plato: "));
	bool operacion_ok = cliente_enviar_mensaje(cliente, AGREGAR_PLATO, datos);
	consola_if(consola, operacion_ok);
}

/*GUARDAR PLATO*/
static void guardar_plato()
{
	if (modulo == RESTAURANTE || modulo == APP)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}


	if(validar_restaurante())
		return;


	char* id = consola_leer("Ingrese el id del pedido: ");
	id_pedido = strtol(id, NULL, 10);
	free(id);
	char* plato = consola_leer("Ingrese el nombre de la comida: ");
	char* cant_s = consola_leer("Ingrese la cantidad: ");
	int cant = strtol(cant_s, NULL, 10);
	free(cant_s);

	t_guardar_plato* datos = crear_datos_guardar_plato(id_pedido,cant ,plato , restaurante_seleccionado);

	bool operacion_ok = cliente_enviar_mensaje(cliente, GUARDAR_PLATO, datos);
	consola_if(consola, operacion_ok);
}

/*OBTENER PEDIDO*/
static void obtener_pedido()
{
	if (modulo == RESTAURANTE || modulo == APP)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	if(validar_restaurante())
		return;


	char* id = consola_leer("Ingrese el id del pedido: ");
	id_pedido = strtol(id, NULL, 10);
	free(id);
	t_datos_pedido* datos = crear_datos_pedido(id_pedido, restaurante_seleccionado);

	t_datos_estado_pedido* datos_pedido = cliente_enviar_mensaje(cliente, OBTENER_PEDIDO, datos);

	if (datos_pedido == NULL)
		{consola_log(consola, "No se pudo obtener pedido");
		return;}

	char* mensaje = malloc(200);
	sprintf(mensaje, "El estado del pedido es: %s, y contiene: ", (char*) dictionary_int_get(diccionario_estados, (datos_pedido->estado)));
	consola_log(consola, mensaje);
	free(mensaje);
	void logear_platos(void* plato)
		{t_datos_estado_comida* plato_a_logear = plato;
		char* mensaje2 = malloc(100);
		sprintf(mensaje2, "Comida: %s, cantidad lista: %d, cantidad total %d", plato_a_logear->comida, plato_a_logear->cant_lista, plato_a_logear->cant_total);
		consola_log(consola, mensaje2);
		free(mensaje2);
		}

	list_iterate(datos_pedido->platos, &logear_platos);
}

/*FINALIZAR PEDIDO*/
static void finalizar_pedido()
{

	if (modulo != COMANDA)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	if(validar_restaurante())
		return;

	char* id = consola_leer("Ingrese el id del pedido: ");
			id_pedido = strtol(id, NULL, 10);
			free(id);

	t_datos_pedido* datos = crear_datos_pedido(id_pedido, restaurante_seleccionado);

	bool operacion_ok = cliente_enviar_mensaje(cliente, FINALIZAR_PEDIDO, datos);
	consola_if(consola, operacion_ok);

}

/*CONFIRMAR PEDIDO*/
static void confirmar_pedido()
{

	if (modulo == COMANDA || modulo == SINDICATO)
		{char* id = consola_leer("Ingrese el id del pedido: ");
		id_pedido = strtol(id, NULL, 10);
		free(id);}

	if(validar_restaurante() && validar_pedido())
		return;

	if (modulo == RESTAURANTE)
		{char* id = consola_leer("Ingrese el id del pedido: ");
		id_pedido = strtol(id, NULL, 10);
		free(id);}

	bool operacion_ok = cliente_enviar_mensaje(cliente, CONFIRMAR_PEDIDO, crear_datos_pedido(id_pedido, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
/*
	if (operacion_ok && (modulo == APP  || modulo == RESTAURANTE))
	{
		pthread_t hilo_finalizar_pedido;
		pthread_create(&(hilo_finalizar_pedido), NULL, (void*) &finalizar_pedido, NULL);
		pthread_detach(hilo_finalizar_pedido);
	}*/
}

/*PLATO LISTO*/
static void plato_listo()
{
	if (modulo == RESTAURANTE)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	if(validar_restaurante())
		return;


	if (modulo == COMANDA || modulo == SINDICATO)
		{char* id = consola_leer("Ingrese el id del pedido: ");
		id_pedido = strtol(id, NULL, 10);
		free(id);}

	char* plato = consola_leer("Ingrese el nombre del plato listo: ");
	int cant = 1;
	t_guardar_plato* datos = crear_datos_guardar_plato(id_pedido, cant, plato, restaurante_seleccionado);

	bool operacion_ok = cliente_enviar_mensaje(cliente, PLATO_LISTO, datos);
	consola_if(consola, operacion_ok);
}


/*CONSULTAR PEDIDO*/
static void consultar_pedido()
{
	if (modulo == COMANDA || modulo == SINDICATO)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}


	if(validar_pedido())
		return;

	t_consultar_pedido* datos_pedido = cliente_enviar_mensaje(cliente, CONSULTAR_PEDIDO, (void*) id_pedido);

	if (datos_pedido->estado == ERROR_ESTADO)
		{consola_log(consola, "El pedido ya no existe");
		return;}
	char* mensaje = malloc(200);
	sprintf(mensaje, "El estado del pedido es: %s, pertenece al restaurante %s, y contiene: ", (char*) dictionary_int_get(diccionario_estados, (datos_pedido->estado)), datos_pedido->restaurante);
	consola_log(consola, mensaje);
	free(mensaje);

	void logear_platos(void* plato)
		{t_datos_estado_comida* plato_a_logear = plato;
		char* mensaje2 = malloc(100);
		sprintf(mensaje2, "Comida: %s, cantidad lista: %d, cantidad total %d", plato_a_logear->comida, plato_a_logear->cant_lista, plato_a_logear->cant_total);
		consola_log(consola, mensaje2);
		free(mensaje2);
		}

	list_iterate(datos_pedido->platos, &logear_platos);
}

/*OBTENER RECETA*/
static void obtener_receta()
{
	if (modulo != SINDICATO)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

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

/*OBTENER RESTAURANTE*/
static void obtener_restaurante()
{
	if (modulo != SINDICATO)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

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

/*TERMINAR PEDIDO*/
static void terminar_pedido()
{
	if (modulo != SINDICATO)
	{consola_log(consola, "El modulo no entiende este mensaje");
	return;}

	if(validar_pedido() && validar_restaurante())
		return;

	bool operacion_ok = cliente_enviar_mensaje(cliente, TERMINAR_PEDIDO, crear_datos_pedido(id_pedido, restaurante_seleccionado));
	consola_if(consola, operacion_ok);
}

/*
void handler (int signum)
{
	//consola_log(consola, "El pedido ha finalizado correctamente");

	if (signum == SIGUSR1)
		{consola_log(consola, "El pedido ha finalizado correctamente");
	//	leer_consola();
		}
}

static void esperar_finalizar_pedido()
{
	t_datos_pedido* datos = crear_datos_pedido(id_pedido, restaurante_seleccionado);
	sleep(1);


	pid_t conseguir_tid = syscall(SYS_gettid);
	char* tid = string_itoa(conseguir_tid);
	datos->restaurante = tid;
	signal(SIGUSR1, handler);
	bool operacion_ok = cliente_enviar_mensaje(cliente, FINALIZAR_PEDIDO, datos);


	//exit(-5);
	if (operacion_ok)
		log_info(logger_finalizar_pedido, "El pedido ha finalizado correctamente");
	else log_info(logger_finalizar_pedido, "El pedido no ha podido finalizarse correctamente");
	//	consola_log(consola, "El pedido ha finalizado correctamente");
//	else consola_log(consola, "El pedido no ha podido finalizarse correctamente");
}*/

void inicializar_diccionario_estados ()
{
diccionario_estados = dictionary_int_create();
dictionary_int_put(diccionario_estados, CONFIRMADO, "Confirmado");
dictionary_int_put(diccionario_estados, PENDIENTE, "Pendiente");
dictionary_int_put(diccionario_estados, TERMINADO, "Terminado");
dictionary_int_put(diccionario_estados, ERROR_ESTADO, "Error Estado");
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

