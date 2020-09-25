#include "repartidores.h"



int calcular_distancia (int posicion_x, int posicion_y, int hacia_posicion_x, int hacia_posicion_y)
{
	int distancia = sqrt((hacia_posicion_x - posicion_x) * (hacia_posicion_x - posicion_x) + (hacia_posicion_y - posicion_y) * (hacia_posicion_y - posicion_y));
	return distancia;
}


t_list* agregar_repartidores_a_lista_libre(t_list* lista_repartidores_libres, char** repartidores, char** tiempos_de_descanso, char** frecuencias_de_descanso)
{
	int i;
	for (i=0; repartidores[i] != NULL; i++)
	{
		t_repartidor* repartidor = malloc(sizeof(t_repartidor));

			//SEPARO POSICIONES X E Y DE REPARTIDOR

		char** token;

		token = string_n_split(repartidores[i], 2, "|");

		repartidor->posicion = malloc(sizeof(t_posicion));

		repartidor->posicion->x = (int)strtol (token[0], NULL, 10);
		repartidor->posicion->y = (int)strtol (token[1], NULL, 10);


		repartidor->frecuencia_descanso = (int)strtol (frecuencias_de_descanso[i], NULL, 10);
		repartidor->tiempo_descanso = (int)strtol (tiempos_de_descanso[i], NULL, 10);

		list_add(lista_repartidores_libres, repartidor);

	}


	return lista_repartidores_libres;
}

t_repartidor* sacar_repartidor_mas_cercano(t_list* lista_repartidores_libres, t_posicion* posicion_buscada)
{
	int tamanio_lista = list_size(lista_repartidores_libres);
	int i = 0;
	int j = 0;

	t_repartidor* repartidor_a_probar;
	repartidor_a_probar = list_get(lista_repartidores_libres, i);
	int distancia_minima = calcular_distancia(repartidor_a_probar->posicion->x,repartidor_a_probar->posicion->y, posicion_buscada->x, posicion_buscada->y);

	for (i=1; i < tamanio_lista; i++)
	{
		repartidor_a_probar = list_get(lista_repartidores_libres, i);
		int distancia_minima_para_comparar = calcular_distancia(repartidor_a_probar->posicion->x,repartidor_a_probar->posicion->y, posicion_buscada->x, posicion_buscada->y);
		if (distancia_minima_para_comparar < distancia_minima)
		{
			j = i;
			distancia_minima = distancia_minima_para_comparar;
		}

	}

	repartidor_a_probar = list_remove(lista_repartidores_libres, j);

	return repartidor_a_probar;
}









t_repartidor* mover_repartidor_hacia(t_repartidor* repartidor, t_posicion* hacia_posicion)
{
	int hacia_posicion_x = hacia_posicion->x;
	int hacia_posicion_y = hacia_posicion->y;

	if (repartidor->posicion->x > hacia_posicion_x){
		repartidor->posicion->x = repartidor->posicion->x - 1;
	}else if (repartidor->posicion->x < hacia_posicion_x){
		repartidor->posicion->x = repartidor->posicion->x + 1;
	}else if (repartidor->posicion->y < hacia_posicion_y){
		repartidor->posicion->y = repartidor->posicion->y + 1;
	}else if (repartidor->posicion->y > hacia_posicion_y){
		repartidor->posicion->y = repartidor->posicion->y - 1;
	}
	return repartidor;
}



