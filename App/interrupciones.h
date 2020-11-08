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

typedef struct {
	int id_pedido;
	t_posicion* posicion_de_restaurante;
	t_posicion* posicion_cliente;
	bool resto_default;
}t_para_crear_pedido;


typedef void* (*t_interrupcion)(t_datos_interrupcion* datos_interrupcion);

void ejecutar_interrupciones();
void inicializar_interrupciones();
void agregar_interrupcion(t_tipo_interrupcion tipo, void* datos);

#endif /*INTERRUPCION_H_*/
