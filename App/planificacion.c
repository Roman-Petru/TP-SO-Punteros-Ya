#include "planificacion.h"


t_list* cola_NEW;
t_list* cola_READY;
t_list* cola_BLOCKED;
t_list* cola_EXEC;
t_list* cola_EXIT;




void planificador_largo_plazo()
{
	while(list_size(cola_NEW) != 0)
	{

	    t_pedido* pedido = list_get(cola_NEW, 0);
		if (list_size(lista_repartidores_libres) != 0)
			{
			t_repartidor* repartidor_cercano = encontrar_repartidor_mas_cercano(lista_repartidores_libres, pedido->posicion_de_restaurante);
			
			//falta sacar repartidor de lista
			
			pedido->repartidor = repartidor_cercano;
			pedido->estado_pcb = READY;
			list_remove(cola_NEW, 0);
			list_add(cola_READY, pedido);  //deberia aniadir dependiendo algoritmo de plani
			}
		else
			{
			break;
			}

	}


}



