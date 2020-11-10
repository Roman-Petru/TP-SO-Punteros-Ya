#include "posicion.h"

t_posicion* posicion_crear(int x, int y)
{
	t_posicion* posicion = malloc(sizeof(t_posicion));
	posicion->x = x;
	posicion->y = y;

	return posicion;
}

t_posicion* posicion_copiar(t_posicion* posicion)
{
	t_posicion* posicion_copia = malloc(sizeof(t_posicion));
	posicion_copia->x = posicion->x;
	posicion_copia->y = posicion->y;

	return posicion_copia;
}

bool posicion_es_igual(t_posicion* posicion_A, t_posicion* posicion_B)
{
	return (posicion_A->x == posicion_B->x) && (posicion_A->y == posicion_B->y);
}

int posicion_distancia_entre(t_posicion* posicion_A, t_posicion* posicion_B)
{
	int distancia_x = abs(posicion_A->x - posicion_B->x);
	int distancia_y = abs(posicion_A->y - posicion_B->y);

	return distancia_x + distancia_y; // Distancia relativa
}
/*
t_posicion* posicion_distancia_minima(t_posicion* posicion, t_list* posiciones)
{
	t_posicion* posicion_menor_distancia(t_posicion* posicion_A, t_posicion* posicion_B)
	{
		int distancia_A = posicion_distancia_entre(posicion, posicion_A);
		int distancia_B = posicion_distancia_entre(posicion, posicion_A);

		return distancia_A > distancia_B ? posicion_B : posicion_A;
	}

	return list_fold(posiciones, list_get(posiciones, 0), &posicion_menor_distancia);
}*/

