#include "planificacion.h"


t_list* cola_NEW;
t_list* cola_READY;
t_list* cola_BLOCKED;
t_list* cola_EXEC;
t_list* cola_EXIT;

t_dictionary_int* diccionario_colas;
t_config* config;

static void inicializar_diccionario_colas();

void planificador_largo_plazo()
{
	while(list_size(cola_NEW) != 0)
	{

		if (list_size(lista_repartidores_libres) != 0)
			{
			t_pedido* pedido = list_get(cola_NEW, 0);
			t_repartidor* repartidor_cercano = sacar_repartidor_mas_cercano(lista_repartidores_libres, pedido->posicion_de_restaurante);
			
			pedido->repartidor = repartidor_cercano;
			cambiar_estado_a(pedido, READY);

			}
		else break;

	}


}


void cambiar_estado_a(t_pedido* pedido, ESTADO_PCB estado_a_pasar)
{
	sacar_de_cola_actual(pedido);
	pedido->estado_pcb = estado_a_pasar;
	t_list* cola_nueva = dictionary_int_get(diccionario_colas, estado_a_pasar);
	meter_en_cola(pedido, cola_nueva);
}




int sacar_de_cola_actual(t_pedido* pedido)
{

		t_list* cola = dictionary_int_get(diccionario_colas, pedido->estado_pcb);

		for(int i=0;i<cola->elements_count;i++)
		{
			if(pedido->id_pedido == ((t_pedido*) list_get(cola, i))->id_pedido)
			{
				list_remove(cola, i);
				return 1;
			}
		}
		return -1;
}

void meter_en_cola(t_pedido* pedido, t_list* cola_nueva)
{
	if (cola_nueva == cola_READY)
	{
		meter_en_cola_READY(pedido);
	} else {
		list_add(cola_nueva, pedido);
	}
}


void meter_en_cola_READY(t_pedido* pedido)
{
	if (strcmp(config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION"), "FIFO") == 0)
		meter_con_FIFO(pedido);

}


void meter_con_FIFO(t_pedido* pedido)  {list_add(cola_READY, pedido); }

//-----------DICCIONARIO DE COLAS-----------//
static void inicializar_diccionario_colas()
{
	diccionario_colas = dictionary_int_create();
	dictionary_int_put(diccionario_colas, NEW, cola_NEW);
	dictionary_int_put(diccionario_colas, READY, cola_READY);
	dictionary_int_put(diccionario_colas, BLOCKED, cola_BLOCKED);
	dictionary_int_put(diccionario_colas, READY, cola_EXEC);
	dictionary_int_put(diccionario_colas, EXIT, cola_EXIT);
}

