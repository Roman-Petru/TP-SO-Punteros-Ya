#include "interfaz_restaurante.h"
#include "restaurante.h"

int indice_id_pedido;

void realizar_handshake_con_app()
{

	bool operacion_ok = cliente_enviar_mensaje(cliente, "APP", HANDSHAKE_RESTO_APP, crear_datos_handshake_restaurante_app(config_get_int_value(config_resto, "PUERTO_ESCUCHA"), nombre_restaurante, posicion));

	if (operacion_ok)
		log_info(logger_resto, "Abierto el restaurante %s, se hizo correctamente el handshake con la app", nombre_restaurante);
	else
		log_info(logger_resto, "No se logró hacer el handshake con la app");

}



static t_respuesta* consultar_platos(char* restaurante)
{
	log_info(logger_resto, "Me consultaron los platos y de paso empece 2 milanesas.");

	restaurante = config_get_string_value(config_resto, "NOMBRE_RESTAURANTE");
	//t_list* platos = cliente_enviar_mensaje(cliente, "SINDICATO", CONSULTAR_PLATOS, restaurante);

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

static t_respuesta* crear_pedido()
{
	log_info(logger_resto, "Me pidieron crear pedido.");

	/*void* extra = malloc (sizeof(int));
	int *id_nuevo_pedido = (int*) extra;
	*id_nuevo_pedido = 5;*/

	int id_nuevo_pedido = indice_id_pedido;
	indice_id_pedido ++;

	//bool operacion_ok = cliente_enviar_mensaje(cliente, "SINDICATO", GUARDAR_PEDIDO, id_nuevo_pedido y restaurante);



	return respuesta_crear(CREAR_PEDIDO_RESPUESTA, (void*) id_nuevo_pedido, false);
}

static t_respuesta* agregar_plato(t_agregar_plato* datos_para_agregar_plato)
{
	bool operacion_ok = cliente_enviar_mensaje(cliente, "SINDICATO", GUARDAR_PLATO, datos_para_agregar_plato);

		if (operacion_ok)
			return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) true, true);
		else
			return respuesta_crear(AGREGAR_PLATO_RESPUESTA, (void*) false, true);
}

static t_respuesta* confirmar_pedido(t_datos_pedido* datos_para_confirmar)
{

// cliente_enviar_mensaje ( PEDIRLE AL SINDICATO QUE PASE EL PEDIDO A ESTADO CONFIRMADO)
//t_datos_del_pedido (no eixste todavia) = cliente_enviar_mensaje(cliente, "SINDICATO", OBTENER_PEDIDO, datos_para_confirmar);


	//se analiza la informacion que se recibe y
	//se comienza la planificacion de todos los platos con agregar_interrupcion(NUEVO_PLATO, "")

	//if se confirmo bien
	return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) true, true);
}
/*

static t_respuesta* consultar_pedido(t_datos_pedido* datos_para_confirmar)
{

//t_datos_del_pedido (no eixste todavia) = cliente_enviar_mensaje(cliente, "SINDICATO", OBTENER_PEDIDO, datos_para_confirmar);

	return respuesta_crear(CONSULTAR_PEDIDO_RESPUESTA, t_datos_del_pedido, true);
}

*/

void cargar_interfaz()
{
	serializacion_inicializar();
	indice_id_pedido = 0;

	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
	servidor_agregar_operacion(servidor, CREAR_PEDIDO, &crear_pedido);
	servidor_agregar_operacion(servidor, AGREGAR_PLATO, &agregar_plato);
	servidor_agregar_operacion(servidor, CONFIRMAR_PEDIDO, &confirmar_pedido);
	//servidor_agregar_operacion(servidor, CONSULTAR_PEDIDO, &consultar_pedido);
}

