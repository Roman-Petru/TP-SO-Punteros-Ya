#include "pedido.h"
#include "app.h"

t_dictionary_int* diccionario_acciones;

static t_posicion* pedido_destino(t_pedido* pedido)
{
	return pedido->instruccion_a_realizar == IR_A_RESTAURANTE ? pedido->posicion_de_restaurante : pedido->posicion_cliente;
}

static t_posicion* pedido_posicion(t_pedido* pedido)
{
	return pedido->repartidor->posicion;
}

static void ciclo_pedido(t_pedido* pedido)
{
	while(true)
	{
		sem_wait(&(pedido->mutex));

		//mover_repartidor_del_pedido(pedido);
		repartidor_mover_hacia(pedido->repartidor,  pedido_destino(pedido));
		log_info(logger, "el pedido %d se movio a (%d,%d)", pedido->id_pedido, pedido_posicion(pedido)->x, pedido_posicion(pedido)->y);

		sem_post(&(sem_ciclo));
	}
}

//========== PEDIDO ==========//
t_pedido* crear_pedido(int id, t_posicion* posicion_de_restaurante, t_posicion* posicion_cliente, bool resto_default)
{
	t_pedido* nuevo_pedido = malloc(sizeof(t_pedido));

	nuevo_pedido->id_pedido = id;
	nuevo_pedido->repartidor = NULL;
	nuevo_pedido->posicion_de_restaurante = posicion_de_restaurante;
	nuevo_pedido->posicion_cliente = posicion_cliente;

	nuevo_pedido->ciclos_en_ready = 0;
	nuevo_pedido->estimacion = config_get_int_value(config, "ESTIMACION_INICIAL");

	nuevo_pedido->esta_listo = resto_default ? true: false;

	//nuevo_pedido->estado_pcb = NULL;
	nuevo_pedido->instruccion_a_realizar = IR_A_RESTAURANTE;

	sem_init(&(nuevo_pedido->mutex), 0, 0);
	pthread_create(&(nuevo_pedido->hilo), NULL, (void*) &ciclo_pedido, nuevo_pedido);
	pthread_detach(nuevo_pedido->hilo);

	log_info(logger, "Se recibió un nuevo pedido:");
	log_info(logger, "	Id del pedido: %d", nuevo_pedido->id_pedido);
	log_info(logger, "	Ubicación del cliente: (%d, %d)", nuevo_pedido->posicion_cliente->x, nuevo_pedido->posicion_cliente->y);
	log_info(logger, "	Ubicación del restaurante: (%d, %d)", nuevo_pedido->posicion_de_restaurante->x, nuevo_pedido->posicion_de_restaurante->y);

	return nuevo_pedido;
}

//TODO estimacion pedido
int pedido_estimacion(t_pedido* pedido)
{
	//char* token = config_get_string_value(config_app, "ALPHA");
	//float valor_alpha = convertir_string_en_float(token);
	double valor_alpha = config_get_double_value(config, "ALPHA");

	return valor_alpha * pedido->repartidor->ciclos_viajando + (1 - valor_alpha) * pedido->estimacion;
}

bool pedido_es_mismo(t_pedido* pedido_A, t_pedido* pedido_B)
{
	return pedido_A->id_pedido == pedido_B->id_pedido;
}

t_pedido* crear_pedido_default(int id)
{
	t_posicion* posicion_de_restaurante = posicion_crear(config_get_int_value(config, "POSICION_REST_DEFAULT_X"),config_get_int_value(config, "POSICION_REST_DEFAULT_Y"));
	t_posicion* posicion_de_cliente = posicion_crear(7,4);

	return crear_pedido(id, posicion_de_restaurante, posicion_de_cliente, true);
}

