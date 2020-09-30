#pragma once

typedef enum
{
	//=== CLIENTE/SERVIDOR ===//
	TERMINAR,
	CONSULTAR_RESTAURANTES,
	SELECCIONAR_RESTAURANTE,
	SELECCIONAR_RESTAURANTE_RESPUESTA,
	CONSULTAR_PLATOS,
	CONSULTAR_PLATOS_RESPUESTA,
	//=== ERROR ===//
	ERROR = -1,
	ERROR_DESERIALIZAR_BUFFER = -2
} t_codigo_de_operacion;

typedef struct
{
	t_codigo_de_operacion codigo_operacion;
	void* datos;
} t_respuesta;

typedef t_respuesta* (*t_operacion_servidor)(void* datos);
typedef t_respuesta* (*t_operacion_servidor_simple)();
typedef void (*t_operacion_cliente)(void* datos);
typedef void (*t_operacion_cliente_simple)();

extern t_respuesta* respuesta_crear(t_codigo_de_operacion codigo_operacion, void* datos);
extern void respuesta_destruir(t_respuesta* respuesta);

void operacion_ok();
