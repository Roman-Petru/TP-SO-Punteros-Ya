#include "cliente.h"
#include <commons/log.h>
#include <readline/readline.h>

void leer_consola(t_log* logger);

int main(void)
{
	t_log* logger = log_create("cliente.log", "Cliente", true, LOG_LEVEL_INFO);

	leer_consola(logger);

	log_destroy(logger);
}

void leer_consola(t_log* logger)
{
	char* leido;

	leido = readline(">");

	while(*leido != '\0') {
		log_info(logger, leido);

		leido = readline(">");
	}
}
