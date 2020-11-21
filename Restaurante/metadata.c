#include "metadata.h"

static bool esta_en_lista_afinidad(char* afinidad)
{
	for (int i=0;i<list_size(lista_afinidades);i++)
	{
	t_afinidad* afinidad_nueva = list_get(lista_afinidades, i);
		if (string_equals_ignore_case(afinidad_nueva->plato, afinidad))
				return true;
	}
	return false;
}

static void sumar_cocinero_a_afinidad (char* afinidad)
{
	void sumar_afi(void* afi) {
		t_afinidad* afinidad_nueva = afi;
		if (string_equals_ignore_case(afinidad_nueva->plato, afinidad))
			afinidad_nueva->cantidad_cocineros++;}

	list_iterate(lista_afinidades, &sumar_afi);
}


void obtener_metadata()
{

	//TODO handshake sindicato;

	t_obtener_restaurante* datos_restaurante = cliente_enviar_mensaje(cliente_sind, OBTENER_RESTAURANTE, nombre_restaurante);

	if (datos_restaurante->cantidad_cocineros == 9753)
		{log_info(logger_resto, "No existe el restaurante en el sindicato, cerrando restaurante"); exit (-1);}

	indice_id_pedido = (config_get_int_value(config_resto, "PUERTO_ESCUCHA")-5000)*1000+datos_restaurante->cantidad_pedidos+1;
	cantidad_hornos = datos_restaurante->cantidad_hornos;
	posicion = posicion_crear(datos_restaurante->posicion->x, datos_restaurante->posicion->y);


	lista_afinidades = list_create();


	while (!list_is_empty(datos_restaurante->lista_afinidades))
	{
		char* afinidad = list_remove(datos_restaurante->lista_afinidades, 0);
		if (!esta_en_lista_afinidad(afinidad))
			{t_afinidad* afinidad_nueva = malloc(sizeof(t_afinidad));
			afinidad_nueva->plato = afinidad;
			afinidad_nueva->cantidad_cocineros = 1;
			list_add(lista_afinidades, afinidad_nueva);}
		else
			sumar_cocinero_a_afinidad(afinidad);

		datos_restaurante->cantidad_cocineros--;
	}
	list_destroy(datos_restaurante->lista_afinidades);

	t_afinidad* afinidad_nueva = malloc(sizeof(t_afinidad));
				afinidad_nueva->plato = "general";
				afinidad_nueva->cantidad_cocineros = datos_restaurante->cantidad_cocineros;
				list_add(lista_afinidades, afinidad_nueva);

	//platos y precios no parecen servir para nada (destroy list)

	free(datos_restaurante);

}

bool obtener_recetas(t_list* platos)
{

	for (int i=0; i<list_size(platos);i++)
	{
		t_datos_estado_comida* comida = list_get(platos, i);
		char* dup = string_duplicate(comida->comida);
		t_obtener_receta* pasos_receta = cliente_enviar_mensaje(cliente_sind, OBTENER_RECETA, comida->comida);
		if (list_is_empty(pasos_receta->pasos))
			return false;

		dictionary_put(diccionario_recetas, dup, pasos_receta->pasos);
	}

	return true;
}
