#include "pedidos.h"
#include "app.h"


t_dictionary_int* diccionario_acciones;

t_pedido* crear_pedido(int id, t_posicion* posicion_de_restaurante, t_posicion* posicion_cliente)
{
	t_pedido* nuevo_pedido = malloc(sizeof(t_pedido));

	nuevo_pedido->id_pedido = id;
	nuevo_pedido->repartidor = NULL;
	nuevo_pedido->posicion_de_restaurante = posicion_de_restaurante;
	nuevo_pedido->posicion_cliente = posicion_cliente;


	nuevo_pedido->ciclos_ejecutandose = 0;
	nuevo_pedido->ciclos_descansados = 0;

	nuevo_pedido->estado_pcb = NEW;
	nuevo_pedido->instruccion_a_realizar = IR_A_RESTAURANTE;

	sem_init(nuevo_pedido->mutex, 0, 0);
	pthread_create(&(nuevo_pedido->hilo), NULL, (void*) &ciclo_pedido, nuevo_pedido);
	//pthread_detach(&(nuevo_pedido->hilo));
	return nuevo_pedido;
}



void ciclo_pedido(t_pedido* pedido)
{
	while(1)
	{
	sem_wait(pedido->mutex);
	//((t_accion) dictionary_int_get(diccionario_acciones, pedido->instruccion_a_realizar))(pedido);
	mover_repartidor_del_pedido(pedido);
	pedido->ciclos_ejecutandose++;

	sem_post(semaforo_app);
	}
}

void descansar_repartidor(t_pedido* pedido)
{
	pedido->ciclos_descansados = pedido->ciclos_descansados + 1;
}

static void mover_repartidor_del_pedido(t_pedido* pedido)
{
	t_posicion* posicion_de_restaurante = pedido->posicion_de_restaurante;

	mover_repartidor_hacia(pedido->repartidor,posicion_de_restaurante);


	log_info(logger_app, "el pedido %d se movio a (%d,%d)", pedido->id_pedido, pedido->repartidor->posicion->x, pedido->repartidor->posicion->y);

}

void inicializar_diccionario_acciones()
{
	diccionario_acciones = dictionary_int_create();
	dictionary_int_put(diccionario_acciones, IR_A_RESTAURANTE, &mover_repartidor_del_pedido);
	dictionary_int_put(diccionario_acciones, IR_A_CLIENTE, &mover_repartidor_del_pedido);

}





