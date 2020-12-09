#include "metadata.h"

pthread_mutex_t mutex_pedidos;
t_list* clientes_conectados;
pthread_mutex_t mutex_clientes;

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
	clientes_conectados = list_create();
	pthread_mutex_init(&(mutex_pedidos), NULL);
	pthread_mutex_init(&(mutex_clientes), NULL);
	void* op_ok = cliente_enviar_mensaje(cliente_sind, HANDSHAKE_RESTO_SIND, NULL);
	while (op_ok == NULL)
		{log_info(logger_resto, "Fallo el handshake con el sindicato, se intentara reconectar en 5 segundos");
		sleep(5);
		op_ok = cliente_enviar_mensaje(cliente_sind, HANDSHAKE_RESTO_SIND, NULL);}


	log_info(logger_resto, "Se realizo handshake contra sindicato correctamente");

	t_obtener_restaurante* datos_restaurante = cliente_enviar_mensaje(cliente_sind, OBTENER_RESTAURANTE, nombre_restaurante);

	if (datos_restaurante->cantidad_cocineros == 9753)
		{log_info(logger_resto, "No existe el restaurante en el sindicato, cerrando restaurante"); exit (-1);}

	indice_id_pedido = (config_get_int_value(config_resto, "PUERTO_ESCUCHA")-5000)*1000+datos_restaurante->cantidad_pedidos+1;
	cantidad_hornos = datos_restaurante->cantidad_hornos;
	posicion = posicion_crear(datos_restaurante->posicion->x, datos_restaurante->posicion->y);
	free(datos_restaurante->posicion);

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

	list_destroy_and_destroy_elements(datos_restaurante->lista_precios, &free);
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
			{free(dup);
			return false;}


		dictionary_put(diccionario_recetas, dup, pasos_receta->pasos);
		free(dup);
		free(pasos_receta);
	}

	return true;
}

void agregar_cliente(t_datos_cliente* datos)
{

	t_cliente_conectado* cliente = malloc(sizeof(t_cliente_conectado));
	cliente->pedidos = list_create();
	size_t tam = strlen(datos->id_cliente)+1;
	cliente->id = malloc(tam);
	memcpy(cliente->id, datos->id_cliente, tam);

	tam = strlen(datos->IP)+1;
	char* IP = malloc(tam);
	memcpy(IP, datos->IP, tam);

	tam = strlen(datos->Puerto)+1;
	char* Puerto = malloc(tam);
	memcpy(Puerto, datos->Puerto, tam);

	cliente->cliente = cliente_crear(IP, Puerto);
	pthread_mutex_lock(&mutex_clientes);
	list_add(clientes_conectados, cliente);
	pthread_mutex_unlock(&mutex_clientes);
}


static int cliente_index(char* id_cliente)
{
	if(id_cliente == NULL)
		return -1;

	int index;
	bool encontro = false;
	bool es_mismo_cliente(void* cliente) { return strcmp(((t_cliente_conectado*) cliente)->id, id_cliente)== 0; }

	for(index = 0; !encontro && list_get(clientes_conectados, index) != NULL; index++)
		encontro = es_mismo_cliente(list_get(clientes_conectados, index));

	return encontro ? index-1 : -1;
}

void cliente_agregar_pedido(char* id_cliente, int id_pedido)
{
	pthread_mutex_lock(&mutex_clientes);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	list_add(cliente->pedidos, (void*) id_pedido);
	pthread_mutex_unlock(&mutex_clientes);
}

t_cliente_red* conseguir_cliente_pedido(int id_pedido)
{
	pthread_mutex_lock(&mutex_clientes);
	bool encontro = false;
	int i;
	for (i=0; i<list_size(clientes_conectados) && !encontro;i++)
		{t_cliente_conectado* cliente = list_get(clientes_conectados, i);
		for (int j=0; j<list_size(cliente->pedidos);j++)
			{
			int id_pivot = (int) list_get(cliente->pedidos, j);
			if (id_pivot == id_pedido)
				encontro = true;
			}
		}
	pthread_mutex_unlock(&mutex_clientes);
	t_cliente_conectado* cliente = list_get(clientes_conectados, i-1);
	return cliente->cliente;
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
