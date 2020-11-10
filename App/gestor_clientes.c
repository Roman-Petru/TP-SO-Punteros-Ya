#include "gestor_clientes.h"

t_list* clientes_conectados;
pthread_mutex_t mutex;

void inicializar_gestor_clientes()
{
	clientes_conectados = list_create();

	agregar_cliente(crear_datos_cliente("Cliente_Default", posicion_crear(4, 4)));

	pthread_mutex_init(&mutex, NULL);
}

static int cliente_index(char* id_cliente)
{
	int index;
	bool encontro = false;
	bool es_mismo_cliente(void* cliente) { return strcmp(((t_cliente_conectado*) cliente)->id, id_cliente)== 0; }

	for(index = 0; !encontro && list_get(clientes_conectados, index) != NULL; index++)
		encontro = es_mismo_cliente(list_get(clientes_conectados, index));

	return encontro ? index-1 : -1;
}

void agregar_cliente(t_datos_cliente* datos)
{

	t_cliente_conectado* cliente = malloc(sizeof(t_cliente_conectado));
	cliente->pedidos = list_create();
	cliente->posicion = datos->posicion;
	cliente->id = datos->id_cliente;
	cliente->restaurante_seleccionado = NULL;

	pthread_mutex_lock(&mutex);
	list_add(clientes_conectados, cliente);
	pthread_mutex_unlock(&mutex);

	free(datos);
}

void cliente_agregar_pedido(char* id_cliente, int id_pedido)
{
	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	list_add(cliente->pedidos, (void*) id_pedido);
	pthread_mutex_unlock(&mutex);
}

void cliente_remover_pedido(char* id_cliente, int id_pedido)
{
	bool es_mismo_pedido(void* id) { return (int) id == id_pedido; }
	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	list_remove_by_condition(cliente->pedidos, &es_mismo_pedido);
	pthread_mutex_unlock(&mutex);
}

void cliente_seleccionar_restaurante(char* id_cliente, char* nombre_restaurante)
{
	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	if(cliente->restaurante_seleccionado != NULL)
		free(cliente->restaurante_seleccionado);
	cliente->restaurante_seleccionado = nombre_restaurante;
	pthread_mutex_unlock(&mutex);
}

char* cliente_obtener_restaurante(char* id_cliente)
{
	char* restaurante;

	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	restaurante = cliente->restaurante_seleccionado;
	pthread_mutex_unlock(&mutex);

	return restaurante;
}

t_posicion* cliente_obtener_posicion(char* id_cliente)
{
	t_posicion* posicion_cliente;

	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	posicion_cliente = cliente->posicion;
	pthread_mutex_unlock(&mutex);

	return posicion_cliente;
}

static void destruir_cliente_conectado(t_cliente_conectado* cliente)
{
	list_destroy(cliente->pedidos);
	free(cliente->posicion);
	//free(cliente->id);
	if(cliente->restaurante_seleccionado != NULL)
		free(cliente->restaurante_seleccionado);
	free(cliente);
}

void remover_cliente(char* id_cliente)
{
	pthread_mutex_lock(&mutex);
	list_remove_and_destroy_element(clientes_conectados, cliente_index(id_cliente), (void*) &destruir_cliente_conectado);
	pthread_mutex_unlock(&mutex);
}




