#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int socket_crear(char* ip, char* puerto);
int socket_escucha_crear(char* ip, char* puerto);
int socket_escucha_esperar_mensaje(int socket_escucha);
void socket_cerrar(int socket);

#endif /*SOCKET_H_*/
