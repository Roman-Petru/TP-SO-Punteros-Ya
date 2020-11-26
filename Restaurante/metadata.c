#include "metadata.h"

pthread_mutex_t mutex_pedidos;

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

	lista_pedidos = list_create();
	pthread_mutex_init(&(mutex_pedidos), NULL);
	bool op_ok = cliente_enviar_mensaje(cliente_sind, HANDSHAKE_RESTO_SIND, NULL);

	if (op_ok)
		log_info(logger_resto, "Se realizo handshake contra sindicato correctamente");
	else
		log_info(logger_resto, "No se realizo handshake contra sindicato correctamente");

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
	lista_platos = list_create();
		for (int i=0; i<list_size(datos_restaurante->lista_precios);i++)
		{
			t_precio* precio = list_get(datos_restaurante->lista_precios, i);
			char* plato_del_resto = precio->nombre_plato;
			list_add(lista_platos, plato_del_resto);
		}

	free(datos_restaurante);

}

bool existe_plato(char* nombre_plato_buscado)
{
	bool es_mismo_plato(void* nombre_plato) {	return strcmp(nombre_plato, nombre_plato_buscado)==0; }

	return list_any_satisfy(lista_platos, &es_mismo_plato);
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



void agendar_pedido(int id_pedido, t_datos_estado_pedido* estado_pedido)
{
	int contar_plantos (int semilla, t_datos_estado_comida* estado_plato){
		return semilla + estado_plato->cant_total;	}

	int cantidad_platos =(int) list_fold(estado_pedido->platos, (void*) 0, (void*) &contar_plantos);

	t_platos_listos* pedido = malloc(sizeof(t_platos_listos));
	pedido->id_pedido = id_pedido;
	pedido->platos_total = cantidad_platos;
	pedido->platos_listos = 0;
	pedido->sincronizador = malloc(sizeof(sem_t));
	sem_init(pedido->sincronizador , 0, 0);
	//log_info(logger_resto, "cantidad platos: %d", cantidad_platos);

	pthread_mutex_lock(&mutex_pedidos);
	list_add(lista_pedidos, pedido);
	pthread_mutex_unlock(&mutex_pedidos);
}
