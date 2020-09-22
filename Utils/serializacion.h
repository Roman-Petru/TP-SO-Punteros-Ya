#include "buffer.h"
#include "dictionary_int.h"

extern t_dictionary_int* diccionario_serializaciones;
extern t_dictionary_int* diccionario_deserializaciones;

typedef t_buffer* (*t_serializador)(void* datos);
typedef void* (*t_deserializador)(t_buffer* buffer);

void diccionario_serializaciones_inicializar();
void diccionario_deserializaciones_inicializar();
void diccionario_serializaciones_destruir();
void diccionario_deserializaciones_destruir();
