#include <commons/config.h>
#include<commons/string.h>
#include <commons/collections/list.h>
#include "../Utils/configs.h"
#include "stddef.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

typedef struct{
	int x;
	int y;
}t_posicion;

typedef struct{
	t_posicion* posicion;
	int frecuencia_descanso;
	int tiempo_descanso;
}t_repartidor;


int calcular_distancia (int posicion_x, int posicion_y, int hacia_posicion_x, int hacia_posicion_y);
t_list* agregar_repartidores_a_lista_libre(t_list* lista_repartidores_libres, char** repartidores, char** tiempos_de_descanso, char** frecuencias_de_descanso);
t_repartidor* encontrar_repartidor_mas_cercano(t_list* lista_repartidores_libres, t_posicion* posicion_buscada);

t_repartidor* mover_repartidor_hacia(t_repartidor* repartidor, int hacia_posicion_x, int hacia_posicion_y);
