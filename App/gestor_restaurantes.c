#include "gestor_restaurantes.h"

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

	return encontro ? index : -1;
}

void agregar_restaurante(char* nombre_restaurante, char* puerto) //char* ip
{
	t_restaurante_conectado* restaurante = malloc(sizeof(t_restaurante_conectado));
	restaurante->nombre = nombre_restaurante;
	restaurante->puerto = puerto;
	//restaurante->ip = ip;

	pthread_mutex_lock(&mutex);
	list_add(restaurantes_conectados, restaurante);
	pthread_mutex_unlock(&mutex);
}

char* restaurante_obtener_puerto(char* nombre_restaurante)
{
	char* puerto;

	pthread_mutex_lock(&mutex);
	t_restaurante_conectado* restaurante = list_get(restaurantes_conectados, restaurante_index(nombre_restaurante));
	puerto = restaurante->puerto;
	pthread_mutex_unlock(&mutex);

	return puerto;
}

void remover_restaurante(char* nombre_restaurante)
{
	void destruir_restaurante(void* restaurante_void)
	{
		t_restaurante_conectado* restaurante = restaurante_void;
		free(restaurante->nombre);
		free(restaurante->puerto);
		free(restaurante);
	}

	pthread_mutex_lock(&mutex);
	list_remove_and_destroy_element(restaurantes_conectados, restaurante_index(nombre_restaurante), &destruir_restaurante);
	pthread_mutex_unlock(&mutex);
}
