#include "interrupciones.h"
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

		repartidor_mover_hacia(pedido->repartidor,  pedido_destino(pedido));
		log_info(logger, "El pedido %d se movio a (%d,%d)", pedido->pcb_id, pedido_posicion(pedido)->x, pedido_posicion(pedido)->y);

		sem_post(&(sem_ciclo));
	}
}

//========== PEDIDO ==========//
t_pedido* pedido_crear(int id_pedido)
{
	t_pedido* nuevo_pedido = malloc(sizeof(t_pedido));
	char* restaurante = pedido_obtener_restaurante(id_pedido);

	nuevo_pedido->id_pedido = id_pedido;
	indice_pcb_id++;
	nuevo_pedido->pcb_id = indice_pcb_id;

	nuevo_pedido->repartidor = NULL;
	nuevo_pedido->posicion_de_restaurante = restaurante_obtener_posicion(restaurante);
	nuevo_pedido->posicion_cliente = cliente_obtener_posicion(pedido_obtener_cliente(id_pedido));

	nuevo_pedido->ciclos_en_ready = 0;
	nuevo_pedido->estimacion = config_get_int_value(config, "ESTIMACION_INICIAL");

	nuevo_pedido->esta_listo = strcmp(restaurante, "Resto_Default") == 0;
	nuevo_pedido->instruccion_a_realizar = IR_A_RESTAURANTE;

	sem_init(&(nuevo_pedido->mutex), 0, 0);
	pthread_create(&(nuevo_pedido->hilo), NULL, (void*) &ciclo_pedido, nuevo_pedido);
	pthread_detach(nuevo_pedido->hilo);

	return nuevo_pedido;
}

void pedido_destruir(t_pedido* pedido)
{
	free(pedido);
}

float pedido_estimacion(t_pedido* pedido)
{
	char* token = config_get_string_value(config, "ALPHA");
	float valor_alpha = convertir_string_en_float(token);
	//double valor_alpha = config_get_double_value(config, "ALPHA");

	return valor_alpha * pedido->repartidor->ciclos_viajando + (1 - valor_alpha) * pedido->estimacion;
}

bool pedido_es_mismo(t_pedido* pedido_A, t_pedido* pedido_B)
{
	return pedido_A->pcb_id == pedido_B->pcb_id;
}

void pedido_actualizar_estado(int id_pedido, t_estado_pedido estado)
{
	//TODO: Pedido_Actualizar_Estado
}

void recibir_pedidos_default(int cantidad)
{
	for (int i=0; i < cantidad; i++)
	{
		vincular_pedido(i, "Cliente_Default", "Resto_Default");
		agregar_interrupcion(NUEVO_PEDIDO, pedido_crear(i));
	}
}

