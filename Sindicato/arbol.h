#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>

typedef struct {
	char* nombre;
	t_list* hijos;
}t_nodo_arbol;

t_nodo_arbol* crearNodo(char* puntoMontaje);
void agregarNodo(t_nodo_arbol* padre, t_nodo_arbol* hijo);
