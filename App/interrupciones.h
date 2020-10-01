#ifndef INTERRUPCION_H_
#define INTERRUPCION_H_

typedef enum {
	NUEVO_PEDIDO
}t_tipo_interrupcion;

typedef struct {
	t_tipo_interrupcion tipo;
	void* datos;
}t_datos_interrupcion;

typedef void* (*t_interrupcion)(t_datos_interrupcion* datos_interrupcion);

void ejecutar_interrupciones();
void inicializar_interrupciones();

#endif /*INTERRUPCION_H_*/
