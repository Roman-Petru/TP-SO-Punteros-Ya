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

// ===== Estructuras ===== //
t_datos_pedido* crear_datos_pedido(int id_pedido, char* restaurante)
{
	t_datos_pedido* datos = malloc(sizeof(t_datos_pedido));
	datos->id_pedido = id_pedido;
	datos->restaurante = restaurante;
	return datos;
}

t_datos_seleccion_restaurante* crear_datos_seleccion_restaurante(int id_cliente, char* restaurante)
{
	t_datos_seleccion_restaurante* datos = malloc(sizeof(t_datos_seleccion_restaurante));
	datos->id_cliente = id_cliente;
	datos->restaurante = restaurante;
	return datos;
}


t_handshake_resto_app* crear_datos_handshake_restaurante_app(int puerto, char* restaurante, t_posicion* posicion)
{
	t_handshake_resto_app* datos = malloc(sizeof(t_handshake_resto_app));
	datos->puerto = puerto;
	datos->restaurante = restaurante;
	datos->posicion = posicion;
	return datos;
}


