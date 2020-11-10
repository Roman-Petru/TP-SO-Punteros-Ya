#include "gestor_restaurantes.h"
#include "app.h"

t_list* restaurantes_conectados;
pthread_mutex_t mutex;

t_list* obtener_restaurantes_conectados()
{
	char* restaurante_nombre(t_restaurante_conectado* resto) { return resto->nombre; }
	return list_map(restaurantes_conectados, (void*) &restaurante_nombre);
}

void inicializar_gestor_restaurantes()
{
	restaurantes_conectados = list_create();
	agregar_restaurante("Resto_Default", posicion_crear(config_get_int_value(config, "POSICION_REST_DEFAULT_X"), config_get_int_value(config, "POSICION_REST_DEFAULT_Y")), NULL);
	pthread_mutex_init(&mutex, NULL);
}

bool hay_restaurantes_conectados() { return !list_is_empty(restaurantes_conectados); }
bool restaurante_esta_conectado(char* nombre_restaurante)
{
	bool es_mismo_restaurante(void* resto) { return strcmp(nombre_restaurante, resto)==0; }
	return list_any_satisfy(restaurantes_conectados, &es_mismo_restaurante);
}

static int restaurante_index(char* nombre_restaurante)
{
	int index;
	bool encontro = false;
	bool es_mismo_restaurante(void* restaurante) { return strcmp(((t_restaurante_conectado*) restaurante)->nombre, nombre_restaurante) == 0; }

	for(index = 0; !encontro && list_get(restaurantes_conectados, index) != NULL; index++)
		encontro = es_mismo_restaurante(list_get(restaurantes_conectados, index));

	return encontro ? index-1 : -1;
}

void agregar_restaurante(char* nombre_restaurante, t_posicion* posicion, char* puerto)
{
	t_restaurante_conectado* restaurante = malloc(sizeof(t_restaurante_conectado));
	restaurante->nombre = nombre_restaurante;
	restaurante->posicion = posicion; //posicion_copiar(posicion);
	restaurante->cliente = cliente_crear("127.0.0.1", puerto);

	pthread_mutex_lock(&mutex);
	list_add(restaurantes_conectados, restaurante);
	pthread_mutex_unlock(&mutex);
}

t_cliente_red* restaurante_obtener_cliente(char* nombre_restaurante)
{
	t_cliente_red* cliente = malloc(sizeof(t_cliente_red));

	pthread_mutex_lock(&mutex);
	t_restaurante_conectado* restaurante = list_get(restaurantes_conectados, restaurante_index(nombre_restaurante));
	cliente = restaurante->cliente;
	pthread_mutex_unlock(&mutex);

	return cliente;
}

t_posicion* restaurante_obtener_posicion(char* nombre_restaurante)
{
	t_posicion* posicion;

	pthread_mutex_lock(&mutex);
	t_restaurante_conectado* restaurante = list_get(restaurantes_conectados, restaurante_index(nombre_restaurante));
	posicion = restaurante->posicion;
	pthread_mutex_unlock(&mutex);

	return posicion;
}

void remover_restaurante(char* nombre_restaurante)
{
	void destruir_restaurante(void* restaurante_void)
	{
		t_restaurante_conectado* restaurante = restaurante_void;
		free(restaurante->nombre);
		free(restaurante->posicion);
		free(restaurante);
	}

	pthread_mutex_lock(&mutex);
	list_remove_and_destroy_element(restaurantes_conectados, restaurante_index(nombre_restaurante), &destruir_restaurante);
	pthread_mutex_unlock(&mutex);
}
