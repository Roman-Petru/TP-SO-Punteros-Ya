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

t_guardar_plato* crear_datos_agregar_plato(int id_pedido, int cantidad, char* comida, char* restaurante)
{
	t_guardar_plato* datos = malloc(sizeof(t_guardar_plato));
	datos->id_pedido = id_pedido;
	datos->cantidad = cantidad;
	datos->comida = comida;
	datos->restaurante = restaurante;
	return datos;
}

t_plato_listo* crear_datos_plato_listo(int id_pedido, char* comida, char* restaurante)
{
	t_plato_listo* datos = malloc(sizeof(t_plato_listo));
	datos->id_pedido = id_pedido;
	datos->comida = comida;
	datos->restaurante = restaurante;
	return datos;
}

t_datos_estado_pedido* crear_datos_estado_pedido(t_estado_pedido estado, t_list* platos)
{
	t_datos_estado_pedido* datos = malloc(sizeof(t_datos_estado_pedido));
	datos->estado = estado;
	datos->platos = platos;

	return datos;
}

t_consultar_pedido* crear_datos_consultar_pedido(char* restaurante, t_estado_pedido estado, t_list* platos)
{
	t_consultar_pedido* datos = malloc(sizeof(t_consultar_pedido));
	datos->restaurante = restaurante;
	datos->estado = estado;
	datos->platos = platos;

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

t_datos_estado_comida* crear_datos_estado_comida(char* comida, uint32_t cant_total, uint32_t cant_lista)
{
	t_datos_estado_comida* datos = malloc(sizeof(t_datos_estado_comida));
	datos->comida = comida;
	datos->cant_total = cant_total;
	datos->cant_lista = cant_lista;
	return datos;
}

t_datos_cliente* crear_datos_cliente(uint32_t id_cliente, t_posicion* posicion)
{
	t_datos_cliente* datos = malloc(sizeof(t_datos_cliente));
	datos->id_cliente = id_cliente;
	datos->posicion = posicion;
	return datos;
}


