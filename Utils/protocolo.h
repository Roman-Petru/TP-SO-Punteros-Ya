#pragma once
#include <stdbool.h>
#include "buffer.h"
#include "dictionary_int.h"

extern t_dictionary_int* diccionario_serializaciones;
extern t_dictionary_int* diccionario_deserializaciones;

typedef enum
{
	//=== CLIENTE/SERVIDOR ===//
	HANDSHAKE_OK,
	OPERACION_OK,
	TERMINAR,
	CONSOLA,
	//=== ERROR ===//
	ERROR = -1,
	ERROR_DESERIALIZAR_BUFFER = -2
} t_codigo_de_operacion;

typedef t_codigo_de_operacion (*t_operacion)(void* datos);

typedef t_buffer* (*t_serializador)(void* datos);
typedef void* (*t_deserializador)(t_buffer* buffer);
void diccionario_serializaciones_inicializar();
void diccionario_deserializaciones_inicializar();
void diccionario_serializaciones_destruir();
void diccionario_deserializaciones_destruir();

void operacion_ok();
