#include "interrupciones.h"
#include "app.h"

t_list* interrupciones;
t_dictionary_int* diccionario_interrupciones;
sem_t mutex_interrupciones;

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
	//free(interrupcion->datos);
	free(interrupcion);
}

static void ejecutar_interrupcion(t_datos_interrupcion* interrupcion)
{
	((t_interrupcion) dictionary_int_get(diccionario_interrupciones, interrupcion->tipo))(interrupcion->datos);
}

//========== LISTA INTERRUPCIONES ==========//
void agregar_interrupcion(t_tipo_interrupcion tipo, void* datos)
{
	sem_wait (&mutex_interrupciones);
	list_add(interrupciones, interrupcion_crear(tipo, datos));
	sem_post (&mutex_interrupciones);
}

void ejecutar_interrupciones()
{
	if(list_is_empty(interrupciones))
		return;
	list_iterate(interrupciones, (void*) &ejecutar_interrupcion);
	list_clean_and_destroy_elements(interrupciones, (void*) &interrupcion_destruir);
}

//========== INTERRUPCIONES ==========//
void interrupcion_nuevo_pedido(t_pedido* pedido) { meter_en_cola(pedido, NEW, A_NEW); }

void interrupcion_terminar_app(void* null) { terminar(); }

void interrupcion_pasar_pedido_a_listo (int id_pedido) {

	void pasar_a_listo(t_pedido* pedido)
		{ if(pedido->id_pedido ==id_pedido)
			pedido->esta_listo = true; }

	list_iterate(cola_EXEC, (void*) &pasar_a_listo);
	list_iterate(cola_READY, (void*) &pasar_a_listo);
	list_iterate(cola_BLOCKED, (void*) &pasar_a_listo);
	list_iterate(cola_NEW, (void*) &pasar_a_listo);
}

//========== DICCIONARIO INTERRUPCIONES ==========//
void inicializar_interrupciones()
{
	sem_init (&(mutex_interrupciones), 0, 1);
	interrupciones = list_create();

	diccionario_interrupciones = dictionary_int_create();
	dictionary_int_put(diccionario_interrupciones, NUEVO_PEDIDO, &interrupcion_nuevo_pedido);
	dictionary_int_put(diccionario_interrupciones, TERMINAR_APP, &interrupcion_terminar_app);
	dictionary_int_put(diccionario_interrupciones, PASAR_A_LISTO, &interrupcion_pasar_pedido_a_listo);
}
