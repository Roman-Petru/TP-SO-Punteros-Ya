#ifndef GESTOR_H
#define GESTOR_H

#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "../Utils/protocolo.h"



void inicializar_gestor_tablas();
bool tabla_restaurante_existe(char* restaurante);
bool segmento_existe(char* restaurante, int id_pedido);
void tabla_restaurante_crear(char* nombre_restaurante);
t_restaurante* obtener_tabla_restaurante(char* restaurante);
t_segmento* obtener_segmento(char* restaurante, int id_pedido);
bool tabla_restaurante_agregar_tabla_segmentos(t_datos_pedido* datos);


#endif
