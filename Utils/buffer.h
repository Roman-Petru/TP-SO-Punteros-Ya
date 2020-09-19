#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct
{
	void* stream;
	uint32_t tamanio;
	uint32_t desplazamiento;
} t_buffer;

t_buffer* buffer_crear(size_t tamanio);
void buffer_destruir(t_buffer* buffer);
void buffer_serializar(t_buffer* buffer, void* datos, size_t tamanio);
void buffer_serializar_string(t_buffer* buffer, char* string);
void* buffer_deserializar(t_buffer* buffer, size_t tamanio);
char* buffer_deserializar_string(t_buffer* buffer);
bool buffer_leido(t_buffer* buffer);
