#ifndef ARBOL_H_
#define ARBOL_H_


#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>


typedef struct {
	char* nombre;
	t_list* hijos;
}t_nodo_arbol;

t_nodo_arbol* crearNodo(char* puntoMontaje);
void agregarNodo(t_nodo_arbol* padre, t_nodo_arbol* hijo);

char* obtener_path_bitmap();
char* obtener_path_bloque(int numero_bloque);
char* obtener_nodo_restaurantes();
char* obtener_nodo_restaurante_especifico(char* nodo_restaurantes, char* restaurante);


#endif
