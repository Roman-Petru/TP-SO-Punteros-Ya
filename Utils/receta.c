#include "receta.h"

t_dictionary* diccionario_recetas;

void inicializar_diccionario_recetas()
{
	diccionario_recetas = dictionary_create();

	//dictionary_put(diccionario_recetas, "guiso", crear_receta());
}

void destruit_diccionario_recetas()
{
	dictionary_destroy_and_destroy_elements(diccionario_recetas, &destruir_receta);
}

t_receta* crear_receta(Paso* pasos, int* tiempos)
{
	t_receta* receta = malloc(sizeof(t_receta));
	receta->pasos = pasos;
	receta->tiempos = tiempos;
	return receta;
}

void destruir_receta(t_receta* receta)
{
	free(receta);
}
