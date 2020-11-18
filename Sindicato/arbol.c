#include "arbol.h"
#include "metadata.h"

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

char* obtener_path_bitmap()
{
	char* nodo = string_new();
	string_append(&nodo, raiz->nombre);
	string_append(&nodo,"/Metadata/Bitmap.bin");

	return nodo;
}

char* obtener_path_bloque(int numero_bloque)
{
	char* string_numero = string_itoa(numero_bloque);
	char* nodo = string_new();
	string_append(&nodo, raiz->nombre);
	string_append(&nodo,"/Blocks/");
	string_append(&nodo, string_numero);
	string_append(&nodo,".AFIP");

	free (string_numero);
	return nodo;
}

char* obtener_nodo_recetas()
{
	char* nodo = string_new();
	string_append(&nodo, raiz->nombre);
	string_append(&nodo,"/Files");
	string_append(&nodo,"/Recetas");

	return nodo;
}

char* obtener_nodo_restaurantes()
{
	char* nodo = string_new();
	string_append(&nodo, raiz->nombre);
	string_append(&nodo,"/Files");
	string_append(&nodo,"/Restaurantes");

	return nodo;
}

char* obtener_nodo_restaurante_especifico(char* restaurante)
{
	char* nodo = string_new();
	string_append(&nodo, obtener_nodo_restaurantes());
	string_append(&nodo,"/");
	string_append(&nodo,restaurante);

	return nodo;
}

char* obtener_path_pedido(char* nodo_restaurante, int id_pedido)
{
	char* nodo = string_new();
	string_append(&nodo, nodo_restaurante);
	string_append(&nodo,"/Pedido");
	char* aux = string_itoa(id_pedido);
	string_append(&nodo, aux);
	string_append(&nodo,".AFIP");

	free(aux);
	return nodo;
}
/*
char* obtener_nodo(char* nodo_buscado)
{

char* aux_buscado = string_new();
string_append(&aux_buscado, raiz->nombre);
bool encontro = false;

void mismo_nodo(void* nodo)
	{
	if(strcmp (nodo_buscado, ((t_nodo_arbol*)nodo)->nombre) == 0)
		{string_append(&aux_buscado, nodo_buscado); encontro = true;}
	}

list_iterate(raiz->hijos, &mismo_nodo);

if (!encontro)
	{string_append(&aux_buscado, "/Files");
	char* nodo_files = obtener_nodo("/Files");
	list_iterate(nodo_files->hijos, &mismo_nodo);
	}
return
}*/
