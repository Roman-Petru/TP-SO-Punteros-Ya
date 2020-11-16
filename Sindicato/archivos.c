#include "archivos.h"


void inicializar_archivos()

{

	char* files = string_new();
	string_append(&files, raiz->nombre);
	string_append(&files,"/Files");
	t_nodo_arbol* nodo_files = crearNodo("/Files");
	agregarNodo(raiz,nodo_files);

	if (!existe_directorio(files))
		mkdir(files, 0700);

	char* restaurantes = string_new();
	string_append(&restaurantes, files);
	string_append(&restaurantes,"/Restaurantes");
	agregarNodo(nodo_files ,crearNodo("/Restaurantes"));

	if (!existe_directorio(restaurantes))
		mkdir(restaurantes, 0700);


	char* recetas = string_new();
	string_append(&recetas, files);
	string_append(&recetas,"/Recetas");
	agregarNodo(nodo_files ,crearNodo("/Recetas"));

	if (!existe_directorio(recetas))
		mkdir(recetas, 0700);

	free(restaurantes);
	free(recetas);
	free(files);

}


