#ifndef POSICION_H_
#define POSICION_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	int x;
	int y;
} t_posicion;

t_posicion* posicion_crear(int x, int y);
t_posicion* posicion_copiar(t_posicion* posicion);
bool posicion_es_igual(t_posicion* posicion_A, t_posicion* posicion_B);
int posicion_distancia_entre(t_posicion* posicion_A, t_posicion* posicion_B);

/*Se destruyen con free*/

#endif /*POSICION_H_*/
