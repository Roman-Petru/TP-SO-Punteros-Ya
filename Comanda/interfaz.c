#include "interfaz.h"
#include "comanda.h"

static t_respuesta* handshake_cliente()
{
	return respuesta_crear(HANDSHAKE_CLIENTE_RESPUESTA, (void*) COMANDA, false);
}

static t_respuesta* guardar_pedido(t_datos_pedido* datos)
{
	if(!tabla_segmento_restaurante_existe(datos->restaurante))
		tabla_restaurante_crear(datos->restaurante); //En caso de no poder crearla, se deberá informar dicha situación.

	return respuesta_crear(GUARDAR_PEDIDO_RESPUESTA, (void*) tabla_restaurante_agregar_segmento(datos), false);
}

static t_respuesta* guardar_plato(t_guardar_plato* datos)
{
	//Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!tabla_segmento_restaurante_existe(datos->restaurante))
		return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);

	//Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!segmento_existe(datos->restaurante, datos->id_pedido))
		return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);

	t_segmento* segmento = obtener_segmento(datos->restaurante, datos->id_pedido);

	if(segmento->estado != PENDIENTE)
		return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);

	//Verificar si existe el plato dentro de la tabla de páginas del pedido. En caso contrario, se deberá asignar una nueva página al segmento.
	if(!pagina_existe(segmento, datos->comida))
		asignar_nueva_pagina(segmento, datos->comida);

	t_pagina* pagina = obtener_pagina(segmento, datos->comida);

	//En caso de que la página que corresponde al pedido no se encuentre cargada en memoria principal, se deberá cargar la misma desde swap (iniciando la elección de víctima de ser necesario). En caso contrario, proceder al paso  número 5.
	bool op_ok = cargar_desde_swap_si_es_necesario(pagina);

	//Se deberá agregar el plato y anexar la cantidad que se tiene que cocinar de dicho plato.
	if(op_ok)
		escribir_marco_principal_guardar_plato(pagina, datos->cantidad);
	else
		{eliminar_pagina_falta_memoria(segmento, pagina);return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);}

	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) op_ok, false);
}

static t_respuesta* obtener_pedido(t_datos_pedido* datos)
{
	//Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!tabla_segmento_restaurante_existe(datos->restaurante))
		return respuesta_crear(OBTENER_PEDIDO_RESPUESTA, NULL, false);

	//Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!segmento_existe(datos->restaurante, datos->id_pedido))
		return respuesta_crear(OBTENER_PEDIDO_RESPUESTA, NULL, false);

	t_segmento* segmento = obtener_segmento(datos->restaurante, datos->id_pedido);

	//En caso de que las páginas que corresponden al pedido no se encuentren cargadas en memoria principal, se deberán cargar las mismas desde swap (iniciando la elección de víctimas de ser necesario). En caso contrario, proceder al paso  número 4.
//	bool op_ok = list_all_satisfy(segmento->tabla_paginas, (void*) cargar_desde_swap_si_es_necesario);
	//list_iterate(segmento->tabla_paginas, (void*) cargar_desde_swap);

//	if(!op_ok)
//		return respuesta_crear(OBTENER_PEDIDO_RESPUESTA, crear_datos_estado_pedido(ERROR, NULL), false);

	//Responder el mensaje indicando si se pudo realizar en conjunto con la información del pedido si correspondiera.

	t_datos_estado_pedido* datos_respuesta = leer_pedido_memoria_principal(segmento);

	return respuesta_crear(OBTENER_PEDIDO_RESPUESTA, (void*) datos_respuesta, false);
}

static t_respuesta* confirmar_pedido(t_datos_pedido* datos)
{
	//Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!tabla_segmento_restaurante_existe(datos->restaurante))
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);

	//Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!segmento_existe(datos->restaurante, datos->id_pedido))
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);

	t_segmento* segmento = obtener_segmento(datos->restaurante, datos->id_pedido);
	//Verificar que el pedido esté en estado “Pendiente”. En caso contrario se deberá informar dicha situación.
	if(segmento->estado != PENDIENTE)
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);

	if (strcmp (datos->restaurante, "Resto_Default") == 0)
		{	segmento->estado = TERMINADO; return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) true, false);}
	//Cambiar el estado del pedido de “Pendiente” a “Confirmado”.
	segmento->estado = CONFIRMADO;

	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) true, false);
}

static t_respuesta* plato_listo(t_guardar_plato* datos)
{
	//Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!tabla_segmento_restaurante_existe(datos->restaurante))
		return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);

	//Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!segmento_existe(datos->restaurante, datos->id_pedido))
		return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);

	t_segmento* segmento = obtener_segmento(datos->restaurante, datos->id_pedido);

	//Verificar si existe el plato dentro de la tabla de páginas del pedido. En caso contrario, se deberá informar dicha situación.
	if(!pagina_existe(segmento, datos->comida))
		return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);

	t_pagina* pagina = obtener_pagina(segmento, datos->comida);

	//Verificar que el pedido esté en estado “Confirmado”. En caso contrario se deberá informar dicha situación.
	if(segmento->estado != CONFIRMADO)
		return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);

	//En caso de que la página que corresponde al pedido no se encuentre cargada en memoria principal, se deberá cargar la misma desde swap (iniciando la elección de víctima de ser necesario). En caso contrario, proceder al paso  número 5.
	bool op_ok = cargar_desde_swap_si_es_necesario(pagina);

	//Se deberá aumentar en uno la cantidad lista de ese plato. En caso de que todos los platos estén listos, se deberá cambiar el estado del pedido a “Terminado”.
	if(op_ok)
		escribir_marco_principal_plato_listo(pagina);
	else
		return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);

	if(list_all_satisfy(segmento->tabla_paginas, (void*) comida_esta_lista))
		segmento->estado = TERMINADO;

	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) op_ok, false);
}

static t_respuesta* finalizar_pedido(t_datos_pedido* datos)
{
	//Verificar si existe la tabla de segmentos de dicho Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!tabla_segmento_restaurante_existe(datos->restaurante))
			return respuesta_crear(FINALIZAR_PEDIDO_RESPUESTA, (void*) false, false);

	//Verificar que exista el segmento de dicho pedido dentro de la tabla de segmentos del Restaurante. En caso de no existir se deberá informar dicha situación.
	if(!segmento_existe(datos->restaurante, datos->id_pedido))
		return respuesta_crear(FINALIZAR_PEDIDO_RESPUESTA, (void*) false, false);

	t_segmento* segmento = obtener_segmento(datos->restaurante, datos->id_pedido);

	//Se deberá proceder a eliminar las páginas correspondientes a dicho segmento.

	eliminar_paginas(segmento);

	//Por último, se procederá a eliminar el segmento correspondiente.

	tabla_restaurante_eliminar_segmento(datos->restaurante, segmento);

	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) true, false);
}

void cargar_interfaz()
{
	servidor_agregar_operacion(servidor, HANDSHAKE_CLIENTE, &handshake_cliente);
	servidor_agregar_operacion(servidor, GUARDAR_PEDIDO, &guardar_pedido);
	servidor_agregar_operacion(servidor, GUARDAR_PLATO, &guardar_plato);
	servidor_agregar_operacion(servidor, OBTENER_PEDIDO, &obtener_pedido);
	servidor_agregar_operacion(servidor, CONFIRMAR_PEDIDO, &confirmar_pedido);
	servidor_agregar_operacion(servidor, PLATO_LISTO, &plato_listo);
	servidor_agregar_operacion(servidor, FINALIZAR_PEDIDO, &finalizar_pedido);
}
