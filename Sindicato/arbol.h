#include arbol.c
#include <commons/collections/list.h>
#include <commons/string.h>

typedef struct {
	char* nombre;
	t_list* hijos;
} NodoArbol;

NodoArbol* crearNodo(char* puntoMontaje);
void agregarNodo(NodoArbol* padre, NodoArbol* hijo);
