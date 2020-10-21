#include "interrupciones.h"

t_list* interrupciones;
t_dictionary_int* diccionario_interrupciones;

static bool ordenar_interrupciones (t_datos_interrupcion* interrupcion1, t_datos_interrupcion* interrupcion2);

//========== INTERRUPCION ==========//
static t_datos_interrupcion* interrupcion_crear(t_tipo_interrupcion tipo, void* datos)
{
	t_datos_interrupcion* interrupcion = malloc(sizeof(t_datos_interrupcion));
	interrupcion->tipo = tipo;
	interrupcion->datos = datos;

	return interrupcion;
}

/*static void interrupcion_destruir(t_datos_interrupcion* interrupcion)
{
	free(interrupcion->datos);
	free(interrupcion);
}*/

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
	list_sort(interrupciones, (void*) &ordenar_interrupciones);
	list_iterate(interrupciones, (void*) &ejecutar_interrupcion);
	//list_clean_and_destroy_elements(interrupciones, (void*) &interrupcion_destruir);
	list_clean(interrupciones);
}

//========== INTERRUPCIONES ==========//
void interrupcion_nuevo_plato(t_para_nuevo_plato* datos)
{
	crear_plato(datos->nombre_plato, datos->id_pedido);
}

void interrupcion_terminar_resto(void* null) { terminar(); }

void interrupcion_terminar_horneado(t_platos_PCB* plato)
{
	plato->hubo_cambio_operacion = false;

	sacar_del_horno(plato);

	t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);

	if (list_is_empty(plato->pasos_receta_faltantes))
	{
		terminar_plato(plato);
		return;
	}

	if (strcmp(paso->operacion, "Hornear") == 0)
		list_add(cola_Hornos_READY, plato);
	if (strcmp(paso->operacion, "Reposar") == 0)
		logear_inicio_operacion(plato);
	if (strcmp(paso->operacion, "Reposar") != 0 && strcmp(paso->operacion, "Hornear") != 0)
		cambiar_estado_a(plato, READY);

}

//========== ORDENAR INTERRUPCIONES ==================//

static bool ordenar_interrupciones (t_datos_interrupcion* interrupcion1, t_datos_interrupcion* interrupcion2)
{
	return (interrupcion1->tipo < interrupcion2->tipo);
}

//========== DICCIONARIO INTERRUPCIONES ==========//
void inicializar_interrupciones()
{
	interrupciones = list_create();

	diccionario_interrupciones = dictionary_int_create();
	dictionary_int_put(diccionario_interrupciones, NUEVO_PLATO, &interrupcion_nuevo_plato);
	dictionary_int_put(diccionario_interrupciones, TERMINAR_RESTO, &interrupcion_terminar_resto);
	dictionary_int_put(diccionario_interrupciones, TERMINAR_HORNEADO, &interrupcion_terminar_horneado);
}
