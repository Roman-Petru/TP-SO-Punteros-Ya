#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "protocolo.h"
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <readline/readline.h>
#include <commons/log.h>
#include <stdlib.h>
#include <stdio.h>


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
bool consola_if(t_consola* consola, bool condicion);

void consola_leer_comando_sindicato(t_consola* consola, char* intro);

#endif /*CONSOLA_H_*/
