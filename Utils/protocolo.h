#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>

typedef enum
{
	//=== CLIENTE/SERVIDOR ===//
	TERMINAR,
	CONEXION_CLIENTE,
	CONEXION_CLIENTE_RESPUESTA,
	CONSULTAR_RESTAURANTES,
	CONSULTAR_RESTAURANTES_RESPUESTA,
	SELECCIONAR_RESTAURANTE,
	SELECCIONAR_RESTAURANTE_RESPUESTA,
	OBTENER_RESTAURANTE,
	OBTENER_RESTAURANTE_RESPUESTA,
	CONSULTAR_PLATOS,
	CONSULTAR_PLATOS_RESPUESTA,
	CREAR_PEDIDO,
	CREAR_PEDIDO_RESPUESTA,
	GUARDAR_PEDIDO,
	GUARDAR_PEDIDO_RESPUESTA,
	AGREGAR_PLATO, //AÑADIR_PLATO
	AGREGAR_PLATO_RESPUESTA, //AÑADIR_PLATO_RESPUESTA
	GUARDAR_PLATO,
	GUARDAR_PLATO_RESPUESTA,
	CONFIRMAR_PEDIDO,
	CONFIRMAR_PEDIDO_RESPUESTA,
	PLATO_LISTO,
	PLATO_LISTO_RESPUESTA,
	TERMINAR_PEDIDO,
	TERMINAR_PEDIDO_RESPUESTA,
	//=== ERROR ===//
	ERROR = -1,
	ERROR_DESERIALIZAR_BUFFER = -2
} t_codigo_de_operacion;

typedef struct
{
	uint32_t id_cliente;
	char* restaurante;
} t_datos_seleccion_restaurante;

typedef struct
{
	t_codigo_de_operacion codigo_operacion;
	void* datos;
	bool destruir_datos;
} t_respuesta;

typedef struct
{
	char* restaurante;
	int id_pedido;
} t_datos_pedido;

typedef struct
{
	char* plato;
	int id_pedido;
} t_agregar_plato;

typedef struct
{
	char* restaurante;
	int id_pedido;
	char* comida;
	int cantidad;
} t_guardar_plato;

typedef struct
{
	char* restaurante;
	int id_pedido;
	char* comida;
} t_plato_listo;

t_datos_pedido* crear_datos_pedido(int id_pedido, char* restaurante);
t_datos_seleccion_restaurante* crear_datos_seleccion_restaurante(int id_cliente, char* restaurante);

typedef t_respuesta* (*t_operacion_servidor)(void* datos);
typedef t_respuesta* (*t_operacion_servidor_simple)();
typedef void (*t_operacion_cliente)(void* datos);
typedef void (*t_operacion_cliente_simple)();

extern t_respuesta* respuesta_crear(t_codigo_de_operacion codigo_operacion, void* datos, bool destruir_datos);
extern void respuesta_destruir(t_respuesta* respuesta);

#endif /*PROTOCOLO_H_*/
