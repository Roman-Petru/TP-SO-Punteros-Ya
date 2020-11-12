#ifndef GESTOR_H
#define GESTOR_H

#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "../Utils/protocolo.h"

typedef struct{
	char* restaurante;
	t_list* tabla_segmentos;
	pthread_mutex_t mutex_tabla_segmentos;
}t_restaurante;

typedef struct{
	int id_pedido;
	t_estado_pedido estado;
	t_list* tabla_paginas;
	pthread_mutex_t mutex_tabla_paginas;
}t_segmento;

typedef struct{
	int id;
	char* comida;
	int marco_principal;
	int marco_virtual;
	bool comida_esta_lista;
	bool uso;
	bool modificado;
	bool validacion_principal;
	bool validacion_virtual;
}t_pagina;


void inicializar_gestor_tablas();

bool tabla_segmento_restaurante_existe(char* restaurante);
bool segmento_existe(char* restaurante, int id_pedido);
bool pagina_existe(t_segmento* segmento, char* comida);

t_restaurante* obtener_tabla_restaurante(char* restaurante);
t_segmento* obtener_segmento(char* restaurante, int id_pedido);
t_pagina* obtener_pagina(t_segmento* segmento, char* comida);

void tabla_restaurante_crear(char* nombre_restaurante);
bool tabla_restaurante_agregar_segmento(t_datos_pedido* datos);
bool asignar_nueva_pagina(t_segmento* segmento, char* comida);

void tabla_restaurante_eliminar_segmento(t_segmento* segmento);
void eliminar_paginas(t_segmento* segmento);
#endif
