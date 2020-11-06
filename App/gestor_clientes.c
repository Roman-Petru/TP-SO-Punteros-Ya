#include "gestor_clientes.h"

t_list* clientes_conectados;
pthread_mutex_t mutex;
int id_cliente;

void inicializar_gestor_clientes()
{
	clientes_conectados = list_create();
	pthread_mutex_init(&mutex, NULL);
	id_cliente = 0;
}

static int cliente_index(int id_cliente)
{
	int index;
	bool encontro = false;
	bool es_mismo_cliente(void* pedido) { return ((t_cliente_conectado*) pedido)->id == id_cliente; }

	for(index = 0; !encontro && list_get(clientes_conectados, index) != NULL; index++)
		encontro = es_mismo_cliente(list_get(clientes_conectados, index));

	return encontro ? index-1 : -1;
}

void agregar_cliente(t_posicion* posicion)
{
	id_cliente++;

	t_cliente_conectado* cliente = malloc(sizeof(t_cliente_conectado));
	cliente->pedidos = list_create();
	cliente->posicion = posicion;
	cliente->id = id_cliente;
	cliente->restaurante_seleccionado = NULL;

	pthread_mutex_lock(&mutex);
	list_add(clientes_conectados, cliente);
	pthread_mutex_unlock(&mutex);
}

void cliente_agregar_pedido(int id_cliente, int id_pedido)
{
	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	list_add(cliente->pedidos, (void*) id_pedido);
	pthread_mutex_unlock(&mutex);
}

void cliente_remover_pedido(int id_cliente, int id_pedido)
{
	bool es_mismo_pedido(void* id) { return (int) id == id_pedido; }
	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	list_remove_by_condition(cliente->pedidos, &es_mismo_pedido);
	pthread_mutex_unlock(&mutex);
}

void cliente_seleccionar_restaurante(int id_cliente, char* nombre_restaurante)
{
	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	if(cliente->restaurante_seleccionado != NULL)
		free(cliente->restaurante_seleccionado);
	cliente->restaurante_seleccionado = nombre_restaurante;
	pthread_mutex_unlock(&mutex);
}

char* cliente_obtener_restaurante(int id_cliente)
{
	char* restaurante;

	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	restaurante = cliente->restaurante_seleccionado;
	pthread_mutex_unlock(&mutex);

	return restaurante;
}

t_posicion* cliente_posicion(int id_cliente)
{
	t_posicion* posicion_cliente;

	pthread_mutex_lock(&mutex);
	t_cliente_conectado* cliente = list_get(clientes_conectados, cliente_index(id_cliente));
	posicion_cliente = cliente->posicion;
	pthread_mutex_unlock(&mutex);

	return posicion_cliente;
}
void remover_cliente(int id_cliente)
{
	void destruir_cliente(void* cliente_void)
	{
		t_cliente_conectado* cliente = cliente_void;
		list_destroy(cliente->pedidos);
		free(cliente);
	}

	pthread_mutex_lock(&mutex);
	list_remove_and_destroy_element(clientes_conectados, cliente_index(id_cliente), &destruir_cliente);
	pthread_mutex_unlock(&mutex);
}




