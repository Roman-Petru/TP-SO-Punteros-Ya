#include "interfaz.h"

static t_respuesta* handshake_cliente()
{
	return respuesta_crear(HANDSHAKE_CLIENTE_RESPUESTA, (void*) SINDICATO, false);
}

static t_respuesta* guardar_pedido(t_datos_pedido* datos)
{
	//Verificar si el Restaurante existe dentro del File System. Para esto se deberá buscar dentro del directorio Restaurantes
	//si existe un subdirectorio con el nombre del Restaurante. En caso de no existir se deberá informar dicha situación.

	if (!restaurante_existe(datos->restaurante))
		return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);

	//Verificar que el ID de pedido no exista para dicho restaurante. 	En caso de existir se deberá informar sobre dicha situación.
	//En caso de que no exista, se deberá crear el archivo.

	char* nodo_resto = obtener_nodo_restaurante_especifico(obtener_nodo_restaurantes(), datos->restaurante);

	if (pedido_existe(nodo_resto, datos->id_pedido))
		{log_info(logger, "No se pudo guardar pedido ya que ya existia");
		return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);	}

	if (crear_archivo_pedido(nodo_resto, datos->id_pedido))
		return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) true, false);
	else return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);
}




void cargar_interfaz()
{
	servidor_agregar_operacion(servidor, HANDSHAKE_CLIENTE, &handshake_cliente);
	servidor_agregar_operacion(servidor, GUARDAR_PEDIDO, &guardar_pedido);
}
