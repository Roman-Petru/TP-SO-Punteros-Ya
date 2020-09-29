#ifndef PLANI_RES
#define PLANI_RES

#include "platos.h"
#include <commons/collections/list.h>

t_list* diccionario_colas;



void cambiar_estado_a(t_platos_PCB* plato, ESTADO_PCB estado_a_pasar);
int sacar_de_cola_actual(t_platos_PCB* pedido);
void meter_en_cola(t_platos_PCB* plato, t_list* cola_nueva);
void meter_en_cola_READY(t_platos_PCB* plato);
int numero_de_cola (char* nombre);
void inicializar_diccionario_colas();


#endif
