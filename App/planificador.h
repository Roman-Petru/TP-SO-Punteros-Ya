#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "app.h"
#include "pedido.h"
#include "repartidor.h"
#include "../Utils/dictionary_int.h"
#include <commons/collections/list.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

typedef enum
{
	A_NEW,
	A_EXEC,

	/*A_READY*/
	A_READY_ASIGNO,
	A_READY_DESCANSO,
	A_READY_LISTO,

	/*A_BLOCKED*/
	A_BLOCKED_CANSADO,
	A_BLOCKED_ESPERA,
	A_BLOCKED_FIN_QUANTUM,

	A_EXIT
}RAZON_CAMBIO_COLA;

extern int indice_pcb_id;

extern t_list* cola_NEW;
extern t_list* cola_READY;
extern t_list* cola_BLOCKED;
extern t_list* cola_EXEC;
extern t_list* cola_EXIT;

typedef void* (*t_insertador)(t_pedido*);

void inicializar_planificador();
void planificar_corto_plazo();
void planificar_largo_plazo();
void cambiar_estado_a(t_pedido* pedido, ESTADO_PCB estado, RAZON_CAMBIO_COLA razon);
void meter_en_cola(t_pedido* pedido, ESTADO_PCB estado, RAZON_CAMBIO_COLA razon);
bool highest_ratio_response(t_pedido* pedido1, t_pedido* pedido2);
void ejecutar_ciclo();
void logear_cambio_cola(t_pedido* pedido, t_list* cola_nueva);
float convertir_string_en_float (char* token);

void recibir_pedidos_default(int cantidad);
#endif /*PLANIFICADOR_H_*/

