#ifndef RESTO
#define RESTO

#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/config.h>
#include <semaphore.h>
#include "../Utils/servidor_red.h"
#include "../Utils/cliente_red.h"
#include "../Utils/posicion.h"
#include <stdio.h>
#include <string.h>
#include "planificacionR.h"
#include "platos.h"
#include "interfaz_restaurante.h"
#include "interrupciones.h"
#include "metadata.h"

extern t_servidor_red* servidor;
extern t_cliente_red* cliente_sind;
extern t_cliente_red* cliente_app;
extern t_log* logger_resto;
extern t_config* config_resto;
extern char* nombre_restaurante;
extern t_posicion* posicion;
extern t_dictionary* diccionario_recetas;
extern t_list* lista_afinidades;
extern t_list* lista_platos;
extern bool app_activada;
extern t_list* lista_pedidos;

extern int cantidad_hornos;
extern int indice_id_pedido;;


typedef struct{
	char* plato;
	int cantidad_cocineros;
}t_afinidad;


void terminar();

#endif //RESTO
