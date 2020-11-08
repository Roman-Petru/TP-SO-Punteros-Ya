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

t_estado_pedido* crear_datos_estado_pedido(bool estado, t_list* platos)
{
	t_estado_pedido* datos = malloc(sizeof(t_estado_pedido));
	datos->estado = estado;
	datos->platos = platos;

	return datos;
}

t_consultar_pedido* crear_datos_consultar_pedido(char* restaurante, bool estado, t_list* platos)
{
	t_consultar_pedido* datos = malloc(sizeof(t_consultar_pedido));
	datos->restaurante = restaurante;
	datos->estado = estado;
	datos->platos = platos;

	return datos;
}
