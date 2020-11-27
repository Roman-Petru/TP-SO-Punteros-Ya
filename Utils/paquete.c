#include "paquete.h"
#include "serializacion.h"
#include <stdlib.h>
#include <string.h>

// ===== Paquete =====
t_paquete* paquete_crear(t_codigo_de_operacion codigo_operacion, t_buffer* buffer)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = buffer;

	return paquete;
}

void paquete_destruir(t_paquete* paquete)
{
	buffer_destruir(paquete->buffer);
	free(paquete);
}

static size_t paquete_tamanio(t_paquete* paquete)
{
	return sizeof(t_codigo_de_operacion) + sizeof(size_t) + paquete->buffer->tamanio;
}

bool paquete_tiene_datos(t_paquete* paquete)
{
	return paquete->buffer->tamanio != 0;
}

void* paquete_desempaquetar(t_paquete* paquete)
{
	return ((t_deserializador) dictionary_int_get(diccionario_deserializaciones, paquete->codigo_operacion))(paquete->buffer);
}

// ===== Serializacion de Paquete =====
static void* paquete_serializar(t_paquete* paquete)
{
	void* paquete_serializado = malloc(paquete_tamanio(paquete));
	t_buffer* buffer = paquete->buffer;

	int desplazamiento = 0;
	memcpy(paquete_serializado + desplazamiento, &(paquete->codigo_operacion), sizeof(t_codigo_de_operacion));
	desplazamiento+= sizeof(t_codigo_de_operacion);
	memcpy(paquete_serializado + desplazamiento, &(buffer->tamanio), sizeof(size_t));
	desplazamiento+= sizeof(size_t);
	memcpy(paquete_serializado + desplazamiento, buffer->stream, buffer->tamanio);
	desplazamiento+= buffer->tamanio;

	return paquete_serializado;
}

// ===== Enviar/Recibir Paquete =====
bool paquete_enviar(t_paquete* paquete, int socket)
{
	void* paquete_serializado = paquete_serializar(paquete);
	int estado = send(socket, paquete_serializado, paquete_tamanio(paquete), 0);
	perror("Perror: ");
	free(paquete_serializado);

	return estado >= 0;
}

t_paquete* paquete_recibir(int numero_socket)
{
	t_paquete* paquete;
	t_buffer* buffer;
	size_t tamanio_buffer;
	t_codigo_de_operacion codigo_operacion;

	if (recv(numero_socket, &codigo_operacion, sizeof(t_codigo_de_operacion), MSG_WAITALL) == -1)
		return NULL;

	if (recv(numero_socket, &tamanio_buffer, sizeof(size_t), MSG_WAITALL) == -1)
		return NULL;

	buffer = buffer_crear(tamanio_buffer);
	if (tamanio_buffer==0)
		return paquete_crear(codigo_operacion, buffer);

	if (recv(numero_socket, buffer->stream, tamanio_buffer, MSG_WAITALL) == -1)
		return NULL;
	paquete = paquete_crear(codigo_operacion, buffer);

	return paquete;
}

