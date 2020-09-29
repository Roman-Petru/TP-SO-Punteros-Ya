#include "planificacionR.h"


void cambiar_estado_a(t_platos_PCB* plato, ESTADO_PCB estado_a_pasar)
{
	sacar_de_cola_actual(plato);
	plato->estado_pcb = estado_a_pasar;
	t_list* cola_nueva = dictionary_int_get(diccionario_colas, plato->estado_pcb);
	meter_en_cola(plato, cola_nueva);
	logear_cambio_cola(plato, cola_nueva);
}




int sacar_de_cola_actual(t_platos_PCB* pedido)
{

		t_list* cola = dictionary_int_get(diccionario_colas, pedido->estado_pcb);


		for(int i=0;i<list_size(cola) ;i++)
		{
			if(pedido->id_pedido == ((t_platos_PCB*) list_get(cola, i))->id_pedido)
			{
				list_remove(cola, i);
				return 1;
			}
		}
		return -1;
}

void meter_en_cola(t_platos_PCB* plato, t_list* cola_nueva)
{
	if (cola_nueva == cola_de_cola_Resto_READY)
		meter_en_cola_READY(plato);
	 else
		list_add(cola_nueva, plato);
}

void meter_en_cola_READY(t_platos_PCB* plato)
{
	int i = numero_de_cola(plato->nombre_plato);

}

int numero_de_cola (char* nombre)
{
	for (int i=0; i<=numero_afinidades;i++)
	{
		if (strcmp(nombre, list_get(lista_afinidades, i)) == 0)
			return i
	}

}

void inicializar_diccionario_colas()
{
	diccionario_colas = dictionary_int_create();
	dictionary_int_put(diccionario_colas, NEW, cola_Resto_NEW);
	dictionary_int_put(diccionario_colas, READY, cola_de_cola_Resto_READY);
	dictionary_int_put(diccionario_colas, BLOCKED, cola_Resto_BLOCKED);
	dictionary_int_put(diccionario_colas, EXEC, cola_Resto_EXEC);
	dictionary_int_put(diccionario_colas, EXIT, cola_Resto_EXIT);
}
