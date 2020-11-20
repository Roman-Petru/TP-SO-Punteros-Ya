#ifndef ARCHIVOS_H_
#define ARCHIVOS_H_

#include "metadata.h"
#include <dirent.h>


int obtener_cantidad_pedidos(char* path_resto);
void modificar_tamanio_real(char* path_archivo, int tamanio);
void inicializar_archivos();
bool restaurante_existe(char* restaurante);
bool pedido_existe(char* nodo_resto, int id_pedido);
bool receta_existe(char* receta);

bool crear_archivo_pedido(char* nodo_resto, int id_pedido);
void crear_receta(char* data);
void crear_restaurante (char* data);

#endif
