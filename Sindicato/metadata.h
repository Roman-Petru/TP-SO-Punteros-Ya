#ifndef METADATA_H_
#define METADATA_H_

#include <stdbool.h>
#include <sys/stat.h>
#include "arbol.h"
#include "sindicato.h"
#include <fcntl.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <sys/mman.h>


typedef struct {
	int block_size;
	int blocks;
}t_metadata;

extern t_nodo_arbol* raiz;
extern t_metadata* metadata;
extern t_bitarray* mapa_bits;



bool existe_directorio(char* path);
bool existe_archivo(char *path);

void crear_bitmap(char* bitm);
void inicializar_metadata();

#endif
