#include "comanda.h"
#include <netdb.h>

t_servidor_red* servidor;
t_log* logger;
t_config* config;
t_list* tabla_restaurantes;
t_list* paginas_en_memoria;

int puntero_a_paginas;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Returns hostname for the local computer
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}

// Returns host information corresponding to host name
void checkHostEntry(struct hostent * hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}

// Converts space-delimited IPv4 addresses
// to dotted-decimal format
void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}



void inicializar_comanda(){
	logger = log_create("comanda.log", "COMANDA", true, LOG_LEVEL_INFO);
	config = config_create("comanda.config");

    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);

    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    checkHostEntry(host_entry);

    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)
                           host_entry->h_addr_list[0]));


	serializacion_inicializar();
	servidor = servidor_crear(IPbuffer, config_get_string_value(config, "PUERTO_ESCUCHA"));
	cargar_interfaz();

	inicializar_memoria_principal();
	inicializar_memoria_virtual();
	inicializar_gestor_tablas();
}


int main()
{
	inicializar_comanda();
	log_info(logger, "Se inicializo la comanda, esperando conecciones de la app");
	while(true)
		sleep(1000000);
}
