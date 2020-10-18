#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#include "buffer.h"
#include "protocolo.h"
#include "dictionary_int.h"
#include <string.h>
#include <stdlib.h>

extern t_dictionary_int* diccionario_serializaciones;
extern t_dictionary_int* diccionario_deserializaciones;
extern t_dictionary_int* diccionario_destrucciones;

typedef t_buffer* (*t_serializador)(void* datos);
typedef void* (*t_deserializador)(t_buffer* buffer);
typedef void (*t_destructor)(void* datos);

void destruir(t_codigo_de_operacion codigo_de_operacion, void* datos);
void destruir_datos_respuesta(t_codigo_de_operacion codigo_de_operacion, void* datos);
void serializacion_inicializar();
void serializacion_finalizar();
void destruir_lista_string(void* datos);

#endif /*SERIALIZACION_H_*/
