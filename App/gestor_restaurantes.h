#ifndef GESTOR_RESTAURANTES_H_
#define GESTOR_RESTAURANTES_H_

#include "../Utils/posicion.h"
#include "../Utils/cliente_red.h"
#include <commons/collections/list.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char* nombre;
	t_posicion* posicion;
	t_cliente_red* cliente;
} t_restaurante_conectado;

void inicializar_gestor_restaurantes();

bool hay_restaurantes_conectados();
bool restaurante_esta_conectado(char* nombre_restaurante);
bool es_resto_default(char* nombre_restaurante);

t_list* obtener_restaurantes_conectados();

void agregar_restaurante(char* nombre_restaurante, t_posicion* posicion, char* puerto);
t_cliente_red* restaurante_obtener_cliente(char* nombre_restaurante);
t_posicion* restaurante_obtener_posicion(char* nombre_restaurante);
void remover_restaurante(char* nombre_restaurante);

#endif /*GESTOR_RESTAURANTES_H_*/
