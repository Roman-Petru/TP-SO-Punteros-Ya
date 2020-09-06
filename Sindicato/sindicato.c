#include "sindicato.h"
#include "Utils/receta.h"
#include <commons/collections/dictionary.h>

int main()
{

}

t_receta* obterner_receta(char* nombre_plato)
{
	return dictionary_get(diccionario_recetas, nombre_plato);
}
