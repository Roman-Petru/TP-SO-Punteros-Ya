#include "interfaz.h"
#include "string_sindicato.h"

static t_respuesta* handshake_cliente()
{
	log_info(logger, "Un cliente realizo un handshake");
	return respuesta_crear(HANDSHAKE_CLIENTE_RESPUESTA, (void*) SINDICATO, false);
}

static t_respuesta* handshake_restaurante()
{
	log_info(logger, "Un restaurante realizo un handshake");
	return respuesta_crear(HANDSHAKE_RESTO_SIND_RESPUESTA, (void*) true, false);
}


static t_respuesta* guardar_pedido(t_datos_pedido* datos)
{
	log_info(logger, "Llego el mensaje %s", __func__);
	//Verificar si el Restaurante existe dentro del File System. Para esto se deberá buscar dentro del directorio Restaurantes
	//si existe un subdirectorio con el nombre del Restaurante. En caso de no existir se deberá informar dicha situación.

	if (!restaurante_existe(datos->restaurante))
	{log_info(logger, "No se pudo guardar pedido ya que no existe el restaurante");
		return respuesta_crear(GUARDAR_PEDIDO_RESPUESTA, (void*) false, false);}

	//Verificar que el ID de pedido no exista para dicho restaurante. 	En caso de existir se deberá informar sobre dicha situación.
	//En caso de que no exista, se deberá crear el archivo.

	char* nodo_resto = obtener_nodo_restaurante_especifico(datos->restaurante);

	if (pedido_existe(nodo_resto, datos->id_pedido))
		{log_info(logger, "No se pudo guardar pedido ya que ya existia");
		return respuesta_crear(GUARDAR_PEDIDO_RESPUESTA, (void*) false, false);	}

	if (crear_archivo_pedido(nodo_resto, datos->id_pedido))
		return respuesta_crear(GUARDAR_PEDIDO_RESPUESTA, (void*) true, false);
	else return respuesta_crear(GUARDAR_PEDIDO_RESPUESTA, (void*) false, false);
}


static t_respuesta* guardar_plato(t_guardar_plato* datos)
{
	log_info(logger, "Llego el mensaje %s", __func__);
	if (!restaurante_existe(datos->restaurante))
	{log_info(logger, "No se pudo guardar plato ya que no existe el restaurante");
		return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);}

	char* nodo_resto = obtener_nodo_restaurante_especifico(datos->restaurante);

	if (!pedido_existe(nodo_resto, datos->id_pedido))
		{log_info(logger, "No se pudo guardar plato ya que no existe el pedido");
		return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);	}

	char* path_pedido = obtener_path_pedido(nodo_resto, datos->id_pedido);

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(path_pedido);

	bool op_ok = mod_string_guardar_plato(datos_para_guardar, datos);
		if(!op_ok)
			return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) false, false);


	guardar_data_en_bloques(datos_para_guardar, path_pedido);

	log_info(logger, "Se guardo plato correctamente");
	return respuesta_crear(GUARDAR_PLATO_RESPUESTA, (void*) true, false);
}

static t_respuesta* confirmar_pedido(t_datos_pedido* datos)
{
	log_info(logger, "Llego el mensaje %s", __func__);
	if (!restaurante_existe(datos->restaurante))
	{log_info(logger, "No se pudo confirmar pedido ya que no existe el restaurante");
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);}

	char* nodo_resto = obtener_nodo_restaurante_especifico(datos->restaurante);

	if (!pedido_existe(nodo_resto, datos->id_pedido))
		{log_info(logger, "No se pudo confirmar pedido ya que no existe el pedido");
		return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);	}

	char* path_pedido = obtener_path_pedido(nodo_resto, datos->id_pedido);

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(path_pedido);

	bool op_ok = mod_string_confirmar_pedido(datos_para_guardar);
		if(!op_ok)
			return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) false, false);


	guardar_data_en_bloques(datos_para_guardar, path_pedido);
	log_info(logger, "Se confirmo pedido correctamente");
	return respuesta_crear(CONFIRMAR_PEDIDO_RESPUESTA, (void*) true, false);
}


static t_respuesta* obtener_pedido(t_datos_pedido* datos)
{
	log_info(logger, "Llego el mensaje %s", __func__);
	if (!restaurante_existe(datos->restaurante))
		{log_info(logger, "No se pudo obtener pedido ya que no existe el restaurante");
		return respuesta_crear(OBTENER_PEDIDO_RESPUESTA, NULL, false);}

	char* nodo_resto = obtener_nodo_restaurante_especifico(datos->restaurante);

	if (!pedido_existe(nodo_resto, datos->id_pedido))
		{log_info(logger, "No se pudo obtener pedido ya que no existe el pedido");
		return respuesta_crear(OBTENER_PEDIDO_RESPUESTA, NULL, false);	}

	char* path_pedido = obtener_path_pedido(nodo_resto, datos->id_pedido);

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(path_pedido);
	t_pedido_sind* pedido = desglosar_pedido(datos_para_guardar->data);

	t_datos_estado_pedido* datos_respuesta = modificar_estructura_pedido(pedido);
	log_info(logger, "Se obtuvo pedido correctamente");
	return respuesta_crear(OBTENER_PEDIDO_RESPUESTA, (void*) datos_respuesta, false);
}


static t_respuesta* plato_listo(t_guardar_plato* datos)
{
	log_info(logger, "Llego el mensaje %s", __func__);
	if (!restaurante_existe(datos->restaurante))
		{log_info(logger, "No se pudo pasar plato listo ya que no existe el restaurante");
		return respuesta_crear(PLATO_LISTO_RESPUESTA, NULL, false);}

	char* nodo_resto = obtener_nodo_restaurante_especifico(datos->restaurante);

	if (!pedido_existe(nodo_resto, datos->id_pedido))
		{log_info(logger, "No se pudo pasar plato listo ya que no existe el pedido");
		return respuesta_crear(PLATO_LISTO_RESPUESTA, NULL, false);	}

	char* path_pedido = obtener_path_pedido(nodo_resto, datos->id_pedido);

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(path_pedido);

	bool op_ok = mod_string_plato_listo(datos_para_guardar, datos);
		if(!op_ok)
			return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) false, false);

	guardar_data_en_bloques(datos_para_guardar, path_pedido);

	log_info(logger, "Se paso plato listo correctamente");
	return respuesta_crear(PLATO_LISTO_RESPUESTA, (void*) op_ok, false);
}


static t_respuesta* terminar_pedido(t_datos_pedido* datos)
{
	log_info(logger, "Llego el mensaje %s", __func__);
	if (!restaurante_existe(datos->restaurante))
	{log_info(logger, "No se pudo terminar pedido ya que no existe el restaurante");
		return respuesta_crear(TERMINAR_PEDIDO_RESPUESTA, (void*) false, false);}

	char* nodo_resto = obtener_nodo_restaurante_especifico(datos->restaurante);

	if (!pedido_existe(nodo_resto, datos->id_pedido))
		{log_info(logger, "No se pudo terminar pedido ya que no existe el pedido");
		return respuesta_crear(TERMINAR_PEDIDO_RESPUESTA, (void*) false, false);	}

	char* path_pedido = obtener_path_pedido(nodo_resto, datos->id_pedido);

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(path_pedido);

	bool op_ok = mod_string_terminar_pedido(datos_para_guardar);
		if(!op_ok)
			return respuesta_crear(TERMINAR_PEDIDO_RESPUESTA, (void*) false, false);


	guardar_data_en_bloques(datos_para_guardar, path_pedido);
	log_info(logger, "Se termino pedido correctamente");
	return respuesta_crear(TERMINAR_PEDIDO_RESPUESTA, (void*) true, false);
}

static t_respuesta* obtener_receta(char* nombre_receta)
{
	log_info(logger, "Llego el mensaje %s", __func__);

	if (!receta_existe(nombre_receta))
		{log_info(logger, "No se pudo obtener receta ya que no existe");
		t_obtener_receta* receta = malloc(sizeof(t_obtener_receta));
		receta->pasos = list_create();
		return respuesta_crear(OBTENER_RECETA_RESPUESTA, (void*) receta, false);}


	char* path_receta = obtener_nodo_recetas();
	string_append(&path_receta, "/");
	string_append(&path_receta, nombre_receta);
	string_append(&path_receta, ".AFIP");

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(path_receta);

	t_obtener_receta* receta = desglosar_receta(datos_para_guardar->data);

	log_info(logger, "Se obtuvo receta correctamente");
	return respuesta_crear(OBTENER_RECETA_RESPUESTA, (void*) receta, false);
}


static t_respuesta* obtener_restaurante(char* nombre_restaurante)
{
	log_info(logger, "Llego el mensaje %s", __func__);

	if (!restaurante_existe(nombre_restaurante))
		{log_info(logger, "No se pudo obtener restaurante ya que no existe");
		t_obtener_restaurante* obtener_res = malloc(sizeof(t_obtener_restaurante));
		obtener_res->cantidad_cocineros = 9753;
		obtener_res->lista_afinidades = list_create();
		obtener_res->lista_precios = list_create();
		obtener_res->posicion = posicion_crear(0, 0);

		return respuesta_crear(OBTENER_RESTAURANTE_RESPUESTA, (void*) obtener_res, false);}


	char* path_restaurante = obtener_nodo_restaurante_especifico(nombre_restaurante);
	int cant_pedidos = obtener_cantidad_pedidos(path_restaurante);
	string_append(&path_restaurante, "/Info.AFIP");

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(path_restaurante);

	t_obtener_restaurante* resto = desglosar_resto(datos_para_guardar->data, cant_pedidos);

	log_info(logger, "Se obtuvo restaurante correctamente");
	return respuesta_crear(OBTENER_RESTAURANTE_RESPUESTA, (void*) resto, false);
}



static t_respuesta* consultar_platos(char* nombre_restaurante)
{
	log_info(logger, "Llego el mensaje %s", __func__);

	if (!restaurante_existe(nombre_restaurante))
	{log_info(logger, "No se pudo consultar plato ya que no existe el restaurante");
	t_list* lista_vac = list_create();
		return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, lista_vac, true);}


	char* path_restaurante = obtener_nodo_restaurante_especifico(nombre_restaurante);
	string_append(&path_restaurante, "/Info.AFIP");

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(path_restaurante);

	t_list* lista_platos = desglosar_platos(datos_para_guardar->data);

	log_info(logger, "Se obtuvieron platos correctamente");
	return respuesta_crear(CONSULTAR_PLATOS_RESPUESTA, lista_platos, true);

}



void cargar_interfaz()
{
	servidor_agregar_operacion(servidor, HANDSHAKE_CLIENTE, &handshake_cliente);
	servidor_agregar_operacion(servidor, HANDSHAKE_RESTO_SIND, &handshake_restaurante);
	servidor_agregar_operacion(servidor, CONSULTAR_PLATOS, &consultar_platos);
	servidor_agregar_operacion(servidor, GUARDAR_PEDIDO, &guardar_pedido);
	servidor_agregar_operacion(servidor, GUARDAR_PLATO, &guardar_plato);
	servidor_agregar_operacion(servidor, CONFIRMAR_PEDIDO, &confirmar_pedido);
	servidor_agregar_operacion(servidor, OBTENER_PEDIDO, &obtener_pedido);
	servidor_agregar_operacion(servidor, PLATO_LISTO, &plato_listo);
	servidor_agregar_operacion(servidor, TERMINAR_PEDIDO, &terminar_pedido);
	servidor_agregar_operacion(servidor, OBTENER_RECETA, &obtener_receta);
	servidor_agregar_operacion(servidor, OBTENER_RESTAURANTE, &obtener_restaurante);
}



