#include "interrupciones.h"
#include "planificador.h"
#include "pedido.h"

t_list* interrupciones;
t_dictionary_int* diccionario_interrupciones;

//========== INTERRUPCION ==========//
static t_datos_interrupcion* interrupcion_crear(t_tipo_interrupcion tipo, void* datos)
{
	t_datos_interrupcion* interrupcion = malloc(sizeof(t_datos_interrupcion));
	interrupcion->tipo = tipo;
	interrupcion->datos = datos;

	return interrupcion;
}

static void interrupcion_destruir(t_datos_interrupcion* interrupcion)
{
	free(interrupcion->datos);
	free(interrupcion);
}

static void ejecutar_interrupcion(t_datos_interrupcion* interrupcion)
{
	((t_interrupcion) dictionary_int_get(diccionario_interrupciones, interrupcion->tipo))(interrupcion->datos);
}

//========== LISTA INTERRUPCIONES ==========//
void agregar_interrupcion(t_tipo_interrupcion tipo, void* datos)
{
	list_add(interrupciones, interrupcion_crear(tipo, datos));
}

void ejecutar_interrupciones()
{
	if(list_is_empty(interrupciones))
		return;
	list_iterate(interrupciones, (void*) &ejecutar_interrupcion);
	list_clean_and_destroy_elements(interrupciones, (void*) &interrupcion_destruir);
}

//========== INTERRUPCIONES ==========//
void interrupcion_nuevo_pedido(t_pedido* pedido)
{
	meter_en_cola(pedido, NEW);
}

//========== DICCIONARIO INTERRUPCIONES ==========//
void inicializar_interrupciones()
{
	diccionario_interrupciones = dictionary_int_create();
	dictionary_int_put(diccionario_interrupciones, NUEVO_PEDIDO, &interrupcion_nuevo_pedido);
}
