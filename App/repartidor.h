#ifndef REPARTIDOR_H_
#define REPARTIDOR_H_

#include "../Utils/posicion.h"
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern t_list* repartidores_libres;

typedef struct {
	t_posicion* posicion;
	int frecuencia_descanso;
	int tiempo_descanso;

	int ciclos_viajando;
	int ciclos_descansados;

	bool esta_cansado;
} t_repartidor;

t_repartidor* repartidor_crear(t_posicion* posicion, int frecuencia_descanso, int tiempo_descanso);
void repartidor_destruir(t_repartidor* repartidor);
void repartidor_mover_hacia(t_repartidor* repartidor, t_posicion* destino);
void repartidor_descansar(t_repartidor* repartidor);

void cargar_repartidores();
t_repartidor* repartidor_obtener_mas_cercano(t_posicion* destino);

#endif /*REPARTIDOR_H_*/
