#include "buffer.h"
#include "dictionary_int.h"

extern t_dictionary_int* diccionario_serializaciones;
extern t_dictionary_int* diccionario_deserializaciones;
extern t_dictionary_int* diccionario_destrucciones;

typedef t_buffer* (*t_serializador)(void* datos);
typedef void* (*t_deserializador)(t_buffer* buffer);
typedef void (*t_destructor)(void* datos);

void diccionario_serializaciones_inicializar();
void diccionario_deserializaciones_inicializar();
void diccionario_serializaciones_destruir();
void diccionario_deserializaciones_destruir();
void diccionario_destrucciones_inicializar();
void diccionario_destrucciones_destruir();
