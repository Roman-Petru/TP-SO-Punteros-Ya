#include "buffer.h"
#include "serializacion.h"

// ===== Buffer =====
t_buffer* buffer_crear(size_t tamanio)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->stream = malloc(tamanio);
	buffer->desplazamiento = 0;
	buffer->tamanio = tamanio;

	return buffer;
}

t_buffer* buffer_crear_con_datos(t_codigo_de_operacion codigo_operacion, void* datos)
{
	if(datos == NULL)
		return buffer_crear(0);
	return ((t_serializador) dictionary_int_get(diccionario_serializaciones, codigo_operacion))(datos);
}

void buffer_destruir(t_buffer* buffer)
{
	free(buffer->stream);
	free(buffer);
}

bool buffer_leido(t_buffer* buffer)
{
	return buffer->tamanio == buffer->desplazamiento;
}

// ===== Serializacion de Buffer =====
void buffer_serializar(t_buffer* buffer, void* datos, size_t tamanio)
{
	memcpy(buffer->stream + buffer->desplazamiento, datos, tamanio);
	buffer->desplazamiento += tamanio;
}

void buffer_serializar_string(t_buffer* buffer, char* string)
{
	size_t tamanio = strlen(string);
	buffer_serializar(buffer, &tamanio, sizeof(size_t));
	buffer_serializar(buffer, string, tamanio);
}


void buffer_serializar_int(t_buffer* buffer, uint32_t integer)
{
	buffer_serializar(buffer, &integer, sizeof(uint32_t));
}

void* buffer_deserializar(t_buffer* buffer, size_t tamanio)
{
	if(buffer->tamanio < buffer->desplazamiento + tamanio)
		exit(ERROR_DESERIALIZAR_BUFFER);

	void* datos = malloc(tamanio);
	memcpy(datos, buffer->stream + buffer->desplazamiento, tamanio);
	buffer->desplazamiento += tamanio;

	return datos;
}

char* buffer_deserializar_string(t_buffer* buffer)
{
	size_t* tamanio = buffer_deserializar(buffer, sizeof(uint32_t));
	char* string = malloc(*tamanio+1);
	if(buffer->tamanio < buffer->desplazamiento + *tamanio)
		exit(ERROR_DESERIALIZAR_BUFFER);
	memcpy(string, buffer->stream + buffer->desplazamiento, *tamanio);
	buffer->desplazamiento += *tamanio;
	string[*tamanio] = '\0';
	free(tamanio);

	return string;
}

uint32_t buffer_deserializar_int(t_buffer* buffer)
{
	uint32_t* integer = buffer_deserializar(buffer, sizeof(uint32_t));;
	return *integer;
}
