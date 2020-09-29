#include "protocolo.h"
#include "buffer.h"
#include "serializacion.h"
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>

// ===== Operaciones =====

// ===== Operaciones Respuesta =====
void handshake_ok()
{
	t_log* logger = log_create("cliente.log", "Cliente", true, LOG_LEVEL_INFO);
	log_info(logger, "HANDSHAKE OK.");
	log_destroy(logger);
}

void operacion_ok()
{
	t_log* logger = log_create("cliente.log", "Cliente", true, LOG_LEVEL_INFO);
	log_info(logger, "OPERACION OK.");
	log_destroy(logger);
}

// ===== Respuesta =====
t_respuesta* respuesta_crear(t_codigo_de_operacion codigo_operacion, void* datos)
{
	t_respuesta* respuesta = malloc(sizeof(t_respuesta));
	respuesta->codigo_operacion = codigo_operacion;
	respuesta->datos = datos;
	return respuesta;
}

void respuesta_destruir(t_respuesta* respuesta)
{
	if(respuesta->datos!=NULL)
		((t_destructor) dictionary_int_get(diccionario_destrucciones, respuesta->codigo_operacion))(respuesta->datos);
	free(respuesta);
}
