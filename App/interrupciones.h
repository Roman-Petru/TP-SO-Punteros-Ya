#ifndef INTERRUPCION_H_
#define INTERRUPCION_H_

#include "planificador.h"
#include "pedido.h"

typedef enum {
	TERMINAR_APP,
	NUEVO_PEDIDO
}t_tipo_interrupcion;

typedef struct {
	t_tipo_interrupcion tipo;
	void* datos;
}t_datos_interrupcion;

typedef void* (*t_interrupcion)(t_datos_interrupcion* datos_interrupcion);

void ejecutar_interrupciones();
void inicializar_interrupciones();
void agregar_interrupcion(t_tipo_interrupcion tipo, void* datos);

#endif /*INTERRUPCION_H_*/
