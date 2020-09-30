#include "buffer.h"
#include "dictionary_int.h"

extern t_dictionary_int* diccionario_serializaciones;
extern t_dictionary_int* diccionario_deserializaciones;
extern t_dictionary_int* diccionario_destrucciones;

typedef t_buffer* (*t_serializador)(void* datos);
typedef void* (*t_deserializador)(t_buffer* buffer);
typedef void (*t_destructor)(void* datos);

void destruir(t_codigo_de_operacion codigo_de_operacion, void* datos);
void serializacion_inicializar();
void serializacion_finalizar();
