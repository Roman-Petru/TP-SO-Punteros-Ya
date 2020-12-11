#include "socket.h"

static struct addrinfo hints_crear()
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	return hints;
}

static struct addrinfo* direccion_crear(char* ip, char* puerto)
{
	struct addrinfo* direccion = NULL;
	struct addrinfo hints = hints_crear();
	getaddrinfo(ip, puerto, &hints, &direccion);

	return direccion;
}

// ========== Socket ==========
int socket_crear(char *ip, char* puerto)
{
	struct addrinfo* server_info = direccion_crear(ip, puerto);
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
		if (socket_cliente == -1)
			{perror("Error de socket:");
			return socket_cliente;	}
	int coneccion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	freeaddrinfo(server_info);
	if (coneccion == -1)
		{perror("Error de connect:");
			return -1;}
	return socket_cliente;
}

int socket_escucha_crear(char* ip, char* puerto)
{
	int socket_servidor;
	struct addrinfo* servinfo = direccion_crear(ip, puerto);

	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if(socket_servidor == -1)
		{perror("Error de socket:");
		abort();}

	int option = 1;

	if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT | SO_LINGER , &option, sizeof(option)) == -1)
		{perror("Error de setsockopt: ");
		abort();}

	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		{perror("Error de bind:");
		abort();}

	if (listen(socket_servidor, SOMAXCONN)== -1)
		{perror("Error de listen:");
		abort();}

    freeaddrinfo(servinfo);

    return socket_servidor;
}

int socket_escucha_esperar_mensaje(int socket_escucha)
{
	socklen_t tam_direccion = sizeof(struct sockaddr_in);
	struct sockaddr_in direccion_cliente;
	int socket_cliente = accept(socket_escucha, (struct sockaddr*)&direccion_cliente, &tam_direccion);

	return socket_cliente;
}

// ========== Finalizar ==========
void socket_cerrar(int socket)
{
	shutdown(socket, SHUT_RDWR);
}

