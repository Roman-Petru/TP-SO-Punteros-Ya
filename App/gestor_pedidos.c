#include "gestor_pedidos.h"

t_list* pedidos_pendientes;
pthread_mutex_t mutex;
int id_pedido;
pthread_mutex_t mutex_id;

void inicializar_gestor_pedidos()
{
	pedidos_pendientes = list_create();
	pthread_mutex_init(&mutex, NULL);
	id_pedido = 770000;
	pthread_mutex_init(&mutex_id, NULL);
}

static int pedido_index(int id_pedido)
{
	int index;
	bool encontro = false;
	bool es_mismo_pedido(void* pedido) { return ((t_pedido_pendiente*) pedido)->id == id_pedido; }

	for(index = 0; !encontro && list_get(pedidos_pendientes, index) != NULL; index++)
		encontro = es_mismo_pedido(list_get(pedidos_pendientes, index));

	return encontro ? index : -1;
}

void vincular_pedido(int id_pedido, char* id_cliente, char* restaurante)
{
	t_pedido_pendiente* pedido = malloc(sizeof(t_pedido_pendiente));
	pedido->id = id_pedido;
	pedido->restaurante = restaurante;
	pedido->id_cliente = id_cliente;

	pthread_mutex_lock(&mutex);
	list_add(pedidos_pendientes, pedido);
	pthread_mutex_unlock(&mutex);

	cliente_agregar_pedido(id_cliente, id_pedido);
}

int generar_id_pedido()
{
	pthread_mutex_lock(&mutex_id);
	id_pedido++;
	pthread_mutex_unlock(&mutex_id);
	return id_pedido;
}

char* pedido_obtener_restaurante(int id_pedido)
{
	char* restaurante;

	bool es_mismo_pedido(void* pedido) { return ((t_pedido_pendiente*) pedido)->id == id_pedido; }

	pthread_mutex_lock(&mutex);
	t_pedido_pendiente* pedido = list_find(pedidos_pendientes, &es_mismo_pedido);
	restaurante = pedido->restaurante;
	pthread_mutex_unlock(&mutex);

	return restaurante;
}

char* pedido_obtener_cliente(int id_pedido)
{
	char* id_cliente;

	bool es_mismo_pedido(void* pedido) { return ((t_pedido_pendiente*) pedido)->id == id_pedido; }

	pthread_mutex_lock(&mutex);
	t_pedido_pendiente* pedido = list_find(pedidos_pendientes, &es_mismo_pedido);
	id_cliente = pedido->id_cliente;
	pthread_mutex_unlock(&mutex);

	return id_cliente;
}

void remover_pedido(int id_pedido)
{
	void destruir_pedido(void* pedido_void)
	{
		t_pedido_pendiente* pedido = pedido_void;
		free(pedido->restaurante);
		free(pedido);
	}

	pthread_mutex_lock(&mutex);
	list_remove_and_destroy_element(pedidos_pendientes, pedido_index(id_pedido), &destruir_pedido);
	pthread_mutex_unlock(&mutex);
}
