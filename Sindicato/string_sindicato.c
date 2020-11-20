#include "string_sindicato.h"


char* remover_corchetes (char* string)
{
	if (strlen(string)<3)
		return string_new();

	char* nuevo_string = malloc(sizeof(strlen(string)-2));
	memcpy(nuevo_string, string+1, strlen(string)-2);
	//free (string); rompe...
	return nuevo_string;
}


static t_datos_estado_comida* cargar_estado_plato(char* plato, char* cant_platos, char* cant_lista)
{

	t_datos_estado_comida* estado_comida = crear_datos_estado_comida(plato, strtol(cant_platos, NULL, 10), strtol(cant_lista, NULL, 10));

	free(cant_platos);
	free(cant_lista);

	return estado_comida;
}

t_pedido_sind* desglosar_pedido (char* pedido_en_string)

{
	t_config_string* string_config = config_string_create(pedido_en_string);

	t_pedido_sind* pedido_sind = malloc(sizeof(t_pedido_sind));
	pedido_sind->platos = list_create();
	pedido_sind->estado = string_duplicate(config_string_get_string_value(string_config, "ESTADO_PEDIDO"));


	char** lista_platos = config_string_get_array_value(string_config, "LISTA_PLATOS");
	char** cant_platos = config_string_get_array_value(string_config, "CANTIDAD_PLATOS");
	char** cant_lista = config_string_get_array_value(string_config, "CANTIDAD_LISTA");


	for (int i=0; lista_platos[i] != NULL; i++)
		{t_datos_estado_comida* estado_comida = cargar_estado_plato(string_duplicate(lista_platos[i]), cant_platos[i], cant_lista[i]);
		list_add(pedido_sind->platos, estado_comida);
		}

	free(lista_platos);
	free(cant_platos);
	free(cant_lista);

	pedido_sind->precio = config_string_get_int_value(string_config, "PRECIO_TOTAL");
	config_string_destroy(string_config);

	return pedido_sind;
}

static t_paso* cargar_pasito(char* operacion, char* tiempo)
{
	t_paso* paso = crear_paso(operacion, strtol(tiempo, NULL, 10));
	free(tiempo);
	return paso;
}


t_obtener_receta* desglosar_receta (char* receta_en_string)

{
	t_config_string* string_config = config_string_create(receta_en_string);

	t_obtener_receta* receta = malloc(sizeof(t_obtener_receta));
	receta->pasos = list_create();

	char** operacion = config_string_get_array_value(string_config, "PASOS");
	char** tiempo_operacion = config_string_get_array_value(string_config, "TIEMPO_PASOS");


	for (int i=0; operacion[i] != NULL; i++)
		{t_paso* paso = cargar_pasito(string_duplicate(operacion[i]), tiempo_operacion[i]);
		list_add(receta->pasos, paso);
		}

	free(operacion);
	free(tiempo_operacion);

	config_string_destroy(string_config);
	return receta;
}

static t_precio* cargar_precio(char* nombre, char* precio)
{
	t_precio* precio_str = crear_precio(nombre, strtol(precio, NULL, 10));
	free(precio);
	return precio_str;
}


t_obtener_restaurante* desglosar_resto (char* resto_en_string, int cant_pedidos)

{
	t_config_string* string_config = config_string_create(resto_en_string);

	t_obtener_restaurante* resto = malloc(sizeof(t_obtener_restaurante));
	resto->lista_afinidades = list_create();

	char** afinidad = config_string_get_array_value(string_config, "AFINIDAD_COCINEROS");

	for (int i=0; afinidad[i] != NULL; i++)
		{char* afi = afinidad[i];
		list_add(resto->lista_afinidades, afi);	}

	resto->lista_precios = list_create();

	char** platos = config_string_get_array_value(string_config, "PLATOS");
	char** precio_platos = config_string_get_array_value(string_config, "PRECIO_PLATOS");

	for (int i=0; platos[i] != NULL; i++)
		{t_precio* precio = cargar_precio(string_duplicate(platos[i]), precio_platos[i]);
		list_add(resto->lista_precios, precio);	}

	resto->cantidad_cocineros = config_string_get_int_value(string_config, "CANTIDAD_COCINEROS");
	resto->cantidad_hornos = config_string_get_int_value(string_config, "CANTIDAD_HORNOS");

	char** posiciones = config_string_get_array_value(string_config, "POSICION");

	resto->posicion = posicion_crear(strtol(posiciones[0], NULL, 10), strtol(posiciones[1], NULL, 10));
	resto->cantidad_pedidos = cant_pedidos;


//	free(platos); free(afinidad);
	free(precio_platos);
//	free(posiciones);free(posiciones[0]);free(posiciones[1]);

	config_string_destroy(string_config);
	return resto;
}

t_list* desglosar_platos (char* resto_en_string)

{
	t_config_string* string_config = config_string_create(resto_en_string);

	t_list* lista_platos = list_create();

	char** platos = config_string_get_array_value(string_config, "PLATOS");


	for (int i=0; platos[i] != NULL; i++)
		{char* plato = platos[i];
		list_add(lista_platos, plato);	}

	config_string_destroy(string_config);
	return lista_platos;
}

int obtener_precio(char* resto_en_string, char* comida)
{
	t_config_string* string_config = config_string_create(resto_en_string);

	int precio;

	char** platos = config_string_get_array_value(string_config, "PLATOS");
	char** precio_platos = config_string_get_array_value(string_config, "PRECIO_PLATOS");

	for (int i=0; platos[i] != NULL; i++)
		if (string_equals_ignore_case(comida, platos[i]))
			precio = strtol(precio_platos[i], NULL, 10);

	config_string_destroy(string_config);
	return precio;
}

bool string_confirmar_pedido(t_pedido_sind* pedido_sind)
{
	if (strcmp(pedido_sind->estado, "Pendiente") != 0)
			{log_info(logger, "No se pudo confirmar el pedido ya que no estaba en estado pendiente");
			return false;	}

	pedido_sind->estado = "Confirmado";
	return true;
}

bool string_terminar_pedido(t_pedido_sind* pedido_sind)
{
	if (strcmp(pedido_sind->estado, "Confirmado") != 0)
			{log_info(logger, "No se pudo terminar el pedido ya que no estaba en estado confirmado");
			return false;	}

	pedido_sind->estado = "Terminado";
	return true;
}

bool string_guardar_plato(t_pedido_sind* pedido_sind, t_guardar_plato* datos_a_guardar)
{
	if (strcmp(pedido_sind->estado, "Pendiente") != 0)
			{log_info(logger, "No se pudo agregar plato porque el pedido no estaba en estado pendiente");
			return false;	}

	bool encontro = false;

	void encontrar_y_agregar_plato (void* estado_comida) {
		t_datos_estado_comida* pedido_pivot = estado_comida;
		if (strcmp(pedido_pivot->comida,datos_a_guardar->comida) == 0)
			{pedido_pivot->cant_total = pedido_pivot->cant_total + datos_a_guardar->cantidad;
			encontro = true;	}
		}

	if (!list_is_empty(pedido_sind->platos))
		list_iterate(pedido_sind->platos, &encontrar_y_agregar_plato);

	if (!encontro)
		{t_datos_estado_comida* estado_comida = crear_datos_estado_comida(datos_a_guardar->comida, datos_a_guardar->cantidad, 0);
		list_add(pedido_sind->platos, estado_comida);
		}


	char* nodo_resto = obtener_nodo_restaurante_especifico(datos_a_guardar->restaurante);
	string_append(&nodo_resto, "/Info.AFIP");

	t_datos_para_guardar* datos_para_guardar = leer_de_bloques(nodo_resto);
	pedido_sind->precio += obtener_precio(datos_para_guardar->data, datos_a_guardar->comida)*datos_a_guardar->cantidad;

	return true;
}



bool string_plato_listo(t_pedido_sind* pedido_sind, t_guardar_plato* datos_a_guardar)
{
	if (strcmp(pedido_sind->estado, "Confirmado") != 0)
			{log_info(logger, "No se pudo pasar a plato listo porque el pedido no estaba en estado pendiente");
			return false;	}

	bool encontro = false;
	bool platos_al_mango = false;
	void encontrar_y_agregar_plato (void* estado_comida) {
		t_datos_estado_comida* pedido_pivot = estado_comida;
		if (strcmp(pedido_pivot->comida,datos_a_guardar->comida) == 0)
			{if (pedido_pivot->cant_lista < pedido_pivot->cant_total)
			{pedido_pivot->cant_lista = pedido_pivot->cant_lista + datos_a_guardar->cantidad;
			encontro = true;	}
			else
			{log_info(logger, "No se pudo pasar plato listo porque ya estan todos los platos de esa comida");
			platos_al_mango = true;}}
		}

	if (!list_is_empty(pedido_sind->platos))
		list_iterate(pedido_sind->platos, &encontrar_y_agregar_plato);

	if (platos_al_mango)
		return false;

	if (!encontro)
		{log_info(logger, "No se pudo pasar a plato listo porque el plato no existe en el pedido"); return false;}

	return true;
}

char* armar_string_pedido(t_pedido_sind* pedido_sind)
{
	char* nuevo_string = string_new();
	string_append(&nuevo_string, "ESTADO_PEDIDO=");
	string_append(&nuevo_string, pedido_sind->estado);
	string_append(&nuevo_string, "\nLISTA_PLATOS=[");

	char* string_platos = string_new();
	char* string_cant_total = string_new();
	char* string_cant_lista = string_new();

	for (int i=0; i < list_size(pedido_sind->platos); i++)
		{t_datos_estado_comida* pedido_pivot = list_get(pedido_sind->platos, i);

		if (i!=0)
			{string_append(&string_platos, ",");
			string_append(&string_cant_total, ",");
			string_append(&string_cant_lista, ",");}

		string_append(&string_platos, pedido_pivot->comida);
		char* aux = string_itoa(pedido_pivot->cant_total);
		string_append(&string_cant_total, aux);
		free(aux);
		char* aux2 = string_itoa(pedido_pivot->cant_lista);
		string_append(&string_cant_lista, aux2);
		free(aux2);
		}

	string_append(&nuevo_string, string_platos);
	string_append(&nuevo_string, "]\nCANTIDAD_PLATOS=[");
	string_append(&nuevo_string, string_cant_total);
	string_append(&nuevo_string, "]\nCANTIDAD_LISTA=[");
	string_append(&nuevo_string, string_cant_lista);
	string_append(&nuevo_string, "]\nPRECIO_TOTAL=");

	char* aux = string_itoa(pedido_sind->precio);
	string_append(&nuevo_string, aux);
	free(aux);

	//destruir estructura?
	return nuevo_string;
}


bool mod_string_guardar_plato(t_datos_para_guardar* datos_para_bloques, t_guardar_plato* datos_a_guardar)
{
	t_pedido_sind* pedido_sind = desglosar_pedido (datos_para_bloques->data);

	bool op_ok = string_guardar_plato(pedido_sind, datos_a_guardar);
		if (!op_ok)
			return false;


	datos_para_bloques->data = armar_string_pedido(pedido_sind);


	int bloques_extra = (strlen(datos_para_bloques->data)/(metadata->block_size-4)-list_size(datos_para_bloques->bloques_siguientes));

	op_ok = (cantidad_bloques_libres()>=bloques_extra);
		if (!op_ok)
			log_info(logger, "No se pudo continuar la operacion ya que no hay mas bloques disponibles");

	return op_ok;
}

bool mod_string_confirmar_pedido(t_datos_para_guardar* datos_para_bloques)
{
	t_pedido_sind* pedido_sind = desglosar_pedido (datos_para_bloques->data);

	bool op_ok = string_confirmar_pedido(pedido_sind);
		if (!op_ok)
			return false;



	datos_para_bloques->data = armar_string_pedido(pedido_sind);

	op_ok = (cantidad_bloques_libres()>=(strlen(datos_para_bloques->data)/(metadata->block_size-4)-list_size(datos_para_bloques->bloques_siguientes)));
		if (!op_ok)
			{log_info(logger, "No se pudo continuar la operacion ya que no hay mas bloques disponibles");
			return false;}

	return true;
}


bool mod_string_plato_listo(t_datos_para_guardar* datos_para_bloques, t_guardar_plato* datos_a_guardar)
{
	t_pedido_sind* pedido_sind = desglosar_pedido (datos_para_bloques->data);

	bool op_ok = string_plato_listo(pedido_sind, datos_a_guardar);
		if (!op_ok)
			return false;



	datos_para_bloques->data = armar_string_pedido(pedido_sind);

	op_ok = (cantidad_bloques_libres()>=(strlen(datos_para_bloques->data)/(metadata->block_size-4)-list_size(datos_para_bloques->bloques_siguientes)));
		if (!op_ok)
			{log_info(logger, "No se pudo continuar la operacion ya que no hay mas bloques disponibles");
			return false;}

	return true;
}

bool mod_string_terminar_pedido(t_datos_para_guardar* datos_para_bloques)
{
	t_pedido_sind* pedido_sind = desglosar_pedido (datos_para_bloques->data);

	bool op_ok = string_terminar_pedido(pedido_sind);
		if (!op_ok)		return false;



	datos_para_bloques->data = armar_string_pedido(pedido_sind);


	op_ok = (cantidad_bloques_libres()>=(strlen(datos_para_bloques->data)/(metadata->block_size-4)-list_size(datos_para_bloques->bloques_siguientes)));
		if (!op_ok)
			{log_info(logger, "No se puede continuar la operacion ya que no hay mas bloques disponibles");
			return false;}

	return true;
}


t_datos_estado_pedido* modificar_estructura_pedido(t_pedido_sind* pedido)
{
	t_estado_pedido estado_pedido;
	if (string_equals_ignore_case("Pendiente", pedido->estado))
		estado_pedido = PENDIENTE;
	if (string_equals_ignore_case("Confirmado", pedido->estado))
		estado_pedido = CONFIRMADO;
	if (string_equals_ignore_case("Terminado", pedido->estado))
		estado_pedido = TERMINADO;

	return crear_datos_estado_pedido(estado_pedido, pedido->platos);
}
