#include "interfaz_restaurante.h"
#include "restaurante.h"

static t_respuesta* consultar_platos()
{
	log_info(logger_resto, "Me consultaron los platos.");

	t_list* platos = cliente_enviar_mensaje(cliente, "SINDICATO", CONSULTAR_PLATOS, config_get_string_value(config_resto, "NOMBRE_RESTAURANTE"));

	return respuesta_crear(CONSULTAR_RESTAURANTES_RESPUESTA, platos, true);
}

void cargar_interfaz()
{
	serializacion_inicializar();

	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
}
