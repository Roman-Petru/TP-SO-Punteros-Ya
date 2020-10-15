#include "consola.h"

static bool consola_tiene_comando(t_consola* consola, char* comando_string)
{
	bool es_mismo_comando(void* comando)
	{
		return strcmp(comando_string, comando)==0;
	}
	return list_any_satisfy(consola->mensajes, &es_mismo_comando);
}

char* consola_leer(char* intro)
{
	return readline(intro);
}

void consola_leer_comando(t_consola* consola, char* intro)
{
	char* leido = readline(intro);

	if(consola_tiene_comando(consola, leido))
		((t_comando) dictionary_get(consola->comandos, leido))();
	else
		log_error(consola->logger, "%s no entiende el mensaje %s.", consola->modulo, leido);
	free(leido);
}

void consola_log(t_consola* consola, char* mensaje)
{
	log_info(consola->logger, mensaje);
}

bool consola_if(t_consola* consola, bool condicion)
{
	if(condicion)
		log_info(consola->logger, "SUCCES!");
	else
		log_info(consola->logger, "ERROR");
	return condicion;
}

t_consola* consola_crear(char* archivo_log, char* modulo)
{
	t_consola* consola = malloc(sizeof(t_consola));
	consola->modulo = modulo;
	consola->logger = log_create(archivo_log, modulo, true, LOG_LEVEL_INFO);
	consola->mensajes = list_create();
	consola->comandos = dictionary_create();
	return consola;
}

void consola_destruir(t_consola* consola)
{
	log_destroy(consola->logger);
	list_destroy(consola->mensajes);
	dictionary_destroy(consola->comandos);
}

void consola_agregar_comando(t_consola* consola, char* mensaje, void* comando)
{
	list_add(consola->mensajes, mensaje);
	dictionary_put(consola->comandos, mensaje,comando);
}




