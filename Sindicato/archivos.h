#ifndef ARCHIVOS_H_
#define ARCHIVOS_H_

#include "metadata.h"
#include <dirent.h>

//ACA VA LA CONEXION CON EL ARBOL

//lISTA DE FUNCIONES DE ARCHIVOS.C

//void cerrar(t_config* conf);  //revisar TODO (falta agregar configuracion)
//void abrir(t_config* conf);

void inicializar_archivos();
bool restaurante_existe(char* restaurante);
bool pedido_existe(char* nodo_resto, int id_pedido);

bool crear_archivo_pedido(char* nodo_resto, int id_pedido);

#endif
