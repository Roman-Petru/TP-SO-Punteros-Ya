#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/log.h>

typedef struct {
	char* modulo;
	t_log* logger;
	t_list* mensajes;
	t_dictionary* comandos;
}t_consola;

typedef void* (*t_comando)();

t_consola* consola_crear(char* archivo_log, char* modulo);
void consola_destruir(t_consola* consola);
char* consola_leer(char* intro);
void consola_leer_comando(t_consola* consola, char* intro);
void consola_log(t_consola* consola, char* mensaje);
void consola_agregar_comando(t_consola* consola, char* mensaje, void* comando);
