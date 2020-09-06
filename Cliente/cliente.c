#include "cliente.h"
#include "Sindicato/sindicato.h"
#include <commons/log.h>

int main(void)
{

}

void leer_consola(t_log* logger)
{
	char* leido;

	//El primero te lo dejo de yapa
	leido = readline(">");

	while(*leido != '\0') {
		log_info(logger, leido);

		leido = readline(">");
	}
}
