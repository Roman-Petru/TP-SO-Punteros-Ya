#include <commons/collections/dictionary.h>

extern t_dictionary* diccionario_recetas;

typedef enum {
	TROCEAR,
	EMPANAR,
	REPOSAR,
	HORNEAR
}Paso;

typedef struct {
	Paso* pasos;
	int* tiempos;
}t_receta;

void inicializar_diccionario_recetas();
void destruit_diccionario_recetas();
