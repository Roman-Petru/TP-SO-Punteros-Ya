#ifndef BLOQUES_H_
#define BLOQUES_H_

#include <string.h>
#include "metadata.h"

void inicializar_bloques();
int reservar_bloque();
void guardar_data_en_bloques(char* data, uint32_t bloque_inicial, t_list* bloques_siguientes);


#endif
