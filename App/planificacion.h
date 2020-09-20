#ifndef PLANI_APP
#define PLANI_APP

#include "repartidores.h"
#include <commons/collections/list.h>
#include <pthread.h>
#include <string.h>
#include "../Utils/dictionary_int.h"
#include <semaphore.h>
#include "app.h"
#include <unistd.h>
#include "pedidos.h"

extern t_list* lista_repartidores_libres;

extern t_list* cola_NEW;
extern t_list* cola_READY;
extern t_list* cola_BLOCKED;
extern t_list* cola_EXEC;
extern t_list* cola_EXIT;


void planificar_corto_plazo();
void planificar_largo_plazo();
void cambiar_estado_a(t_pedido* pedido, ESTADO_PCB estado_a_pasar);
int sacar_de_cola_actual(t_pedido* pedido);
void meter_en_cola(t_pedido* pedido, t_list* cola_nueva);
void meter_en_cola_READY(t_pedido* pedido);
void meter_con_FIFO(t_pedido* pedido);
void ejecutar_ciclo();
void logear_cambio_cola(t_pedido* pedido, t_list* cola_nueva);
void inicializar_diccionario_colas();


#endif //PLANI_APP

