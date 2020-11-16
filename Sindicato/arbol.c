#include "arbol.h"


//ESTRUCTURA DEL ARBOL DE DIRECTORIO
t_nodo_arbol* crearNodo(char* nombre) {

	t_nodo_arbol* nodo = malloc(sizeof(t_nodo_arbol));
	nodo->nombre = string_duplicate(nombre);
	nodo->hijos = list_create();
	return nodo;
}

void agregarNodo(t_nodo_arbol* padre, t_nodo_arbol* hijo) {
	list_add(padre->hijos, hijo);
}

