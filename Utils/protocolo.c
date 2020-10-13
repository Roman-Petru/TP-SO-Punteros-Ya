#include "protocolo.h"
#include "buffer.h"
#include "serializacion.h"

// ===== Respuesta =====
t_respuesta* respuesta_crear(t_codigo_de_operacion codigo_operacion, void* datos, bool destruir_datos)
{
	t_respuesta* respuesta = malloc(sizeof(t_respuesta));
	respuesta->codigo_operacion = codigo_operacion;
	respuesta->datos = datos;
	respuesta->destruir_datos = destruir_datos;

	return respuesta;
}

void respuesta_destruir(t_respuesta* respuesta)
{
	if(respuesta->destruir_datos)
		destruir_datos_respuesta(respuesta->codigo_operacion, respuesta->datos);
	free(respuesta);
}

