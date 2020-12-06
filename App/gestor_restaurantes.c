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
	agregar_restaurante("Resto_Default", posicion_crear(config_get_int_value(config, "POSICION_REST_DEFAULT_X"), config_get_int_value(config, "POSICION_REST_DEFAULT_Y")), "IP de mentira", "Puerto Default");
	pthread_mutex_init(&mutex, NULL);
}

bool hay_restaurantes_conectados() { return !list_is_empty(restaurantes_conectados); }
bool restaurante_esta_conectado(char* nombre_restaurante)
{
	bool es_mismo_restaurante(void* resto) { t_restaurante_conectado* resto_pivot = resto;
		return strcmp(nombre_restaurante, resto_pivot->nombre)==0; }

	return list_any_satisfy(restaurantes_conectados, &es_mismo_restaurante);
}

bool es_resto_default(char* nombre_restaurante) { return strcmp(nombre_restaurante, "Resto_Default")==0; }

static int restaurante_index(char* nombre_restaurante)
{
	int index;
	bool encontro = false;
	bool es_mismo_restaurante(void* restaurante) { return strcmp(((t_restaurante_conectado*) restaurante)->nombre, nombre_restaurante) == 0; }

	for(index = 0; !encontro && list_get(restaurantes_conectados, index) != NULL; index++)
		encontro = es_mismo_restaurante(list_get(restaurantes_conectados, index));

	return encontro ? index-1 : -1;
}

void agregar_restaurante(char* nombre_restaurante, t_posicion* posicion, char* ip, char* puerto)
{
	t_restaurante_conectado* restaurante = malloc(sizeof(t_restaurante_conectado));
	size_t tam = strlen(nombre_restaurante)+1;
	restaurante->nombre = malloc(tam);
	memcpy(restaurante->nombre, nombre_restaurante, tam);
	restaurante->posicion = posicion_copiar(posicion); //posicion_copiar(posicion);
	size_t tam2 = strlen(puerto)+1;
	char* puerto2 = malloc(tam2);
	memcpy(puerto2, puerto, tam2);
	size_t tam3 = strlen(ip)+1;
	char* ip2 = malloc(tam3);
	memcpy(ip2, ip, tam3);
	restaurante->cliente = cliente_crear(ip2, puerto2);

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
