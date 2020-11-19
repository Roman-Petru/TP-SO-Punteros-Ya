#ifndef BLOQUES_H_
#define BLOQUES_H_

#include <string.h>
#include "metadata.h"


typedef struct {
	char* data;
	int bloque_inicial;
	t_list* bloques_siguientes;
}t_datos_para_guardar;


void inicializar_bloques();
int reservar_bloque();
void guardar_data_en_bloques(t_datos_para_guardar* datos_a_guardar, char* path_archivo);
t_datos_para_guardar* leer_de_bloques(char* path);

#endif
