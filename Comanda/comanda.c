#include "comanda.h"
#include <netdb.h>

t_servidor_red* servidor;
t_log* logger;
t_config* config;
t_list* tabla_restaurantes;
t_list* paginas_en_memoria;

int puntero_a_paginas;



void inicializar_comanda(){

	logger = log_create("comanda.log", "COMANDA", true, LOG_LEVEL_INFO);
	config = config_create("comanda.config");

	 /*
	  * #include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

   getifaddrs(&ifAddrStruct);
    char* IP;

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (strcmp("enp0s3", ifa->ifa_name) == 0)
                   {IP = strdup(addressBuffer);
                   break;  }
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }

    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

    log_info(logger, "IP: %s", IP);*/
	serializacion_inicializar();
	servidor = servidor_crear("10.108.64.3", config_get_string_value(config, "PUERTO_ESCUCHA"));
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
