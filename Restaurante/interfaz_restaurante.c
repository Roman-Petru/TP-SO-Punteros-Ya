#include "interfaz_restaurante.h"
#include "restaurante.h"

static t_respuesta* consultar_platos(char* restaurante)
{
	log_info(logger_resto, "Me consultaron los platos y de paso empece 2 milanesas.");

	restaurante = config_get_string_value(config_resto, "NOMBRE_RESTAURANTE");
	//t_list* platos = cliente_enviar_mensaje(cliente, "SINDICATO", CONSULTAR_PLATOS, config_get_string_value(config_resto, "NOMBRE_RESTAURANTE"));

	for (int i=7; i < 9; i++)
	{
	t_para_nuevo_plato* pure = malloc(sizeof(t_para_nuevo_plato));
	pure->nombre_plato = "milanesa";
	pure->id_pedido = i;
	agregar_interrupcion(NUEVO_PLATO, pure);
	}

	t_list* platos = list_create();
	list_add(platos, "milanesa");
	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, platos, false);
}

void cargar_interfaz()
{
	serializacion_inicializar();

	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
}
