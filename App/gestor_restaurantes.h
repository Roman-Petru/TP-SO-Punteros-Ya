#ifndef GESTOR_RESTAURANTES_H_
#define GESTOR_RESTAURANTES_H_

#include <commons/collections/list.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
	char* nombre;
	char* puerto;
	//char* ip;
} t_restaurante_conectado;

bool hay_restaurantes_conectados();
bool restaurante_esta_conectado(char* nombre_restaurante);

t_list* obtener_restaurantes_conectados();
void agregar_restaurante(char* nombre_restaurante, char* puerto);
char* restaurante_obtener_puerto(char* nombre_restaurante);
void remover_restaurante(char* nombre_restaurante);

#endif /*GESTOR_RESTAURANTES_H_*/
