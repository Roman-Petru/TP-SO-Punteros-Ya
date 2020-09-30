#include "posicion.h"
#include <stdlib.h>
//#include <math.h>

t_posicion* posicion_crear(int x, int y)
{
	t_posicion* posicion = malloc(sizeof(t_posicion));
	posicion->x = x;
	posicion->y = y;

	return posicion;
}

bool posicion_es_igual(t_posicion* posicion_A, t_posicion* posicion_B)
{
	return (posicion_A->x == posicion_B->x) && (posicion_A->y == posicion_B->y);
}

int posicion_distancia_entre(t_posicion* posicion_A, t_posicion* posicion_B)
{
	int distancia_x = abs(posicion_A->x - posicion_B->x);
	int distancia_y = abs(posicion_A->y - posicion_B->y);
	//int distancia = sqrt((posicion_A->x - posicion_B->y) * (hacia_posicion_x - posicion_x) + (hacia_posicion_y - posicion_y) * (hacia_posicion_y - posicion_y));
	return distancia_x + distancia_y; // Distancia relativa
}