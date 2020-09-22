#pragma once

int socket_crear(char* ip, char* puerto);
int socket_escucha_crear(char* ip, char* puerto);
int socket_escucha_esperar_mensaje(int socket_escucha);
void socket_cerrar(int socket);
