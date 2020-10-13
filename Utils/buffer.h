#ifndef BUFFER_H_
#define BUFFER_H_

#include "protocolo.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	void* stream;
	uint32_t tamanio;
	uint32_t desplazamiento;
} t_buffer;

t_buffer* buffer_crear(size_t tamanio);
t_buffer* buffer_crear_con_datos(t_codigo_de_operacion codigo_operacion, void* datos);
void buffer_destruir(t_buffer* buffer);
void buffer_serializar(t_buffer* buffer, void* datos, size_t tamanio);
void buffer_serializar_string(t_buffer* buffer, char* string);
void* buffer_deserializar(t_buffer* buffer, size_t tamanio);
char* buffer_deserializar_string(t_buffer* buffer);
bool buffer_leido(t_buffer* buffer);

#endif /*BUFFER_H_*/
