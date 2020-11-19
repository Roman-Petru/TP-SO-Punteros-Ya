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

bool string_confirmar_pedido(t_pedido_sind* pedido_sind)
{
	if (strcmp(pedido_sind->estado, "Pendiente") != 0)
			{log_info(logger, "No se pudo confirmar el pedido ya que no estaba en estado pendiente");
			return false;	}

	pedido_sind->estado = "Confirmado";

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
	return true;

}

bool mod_string_confirmar_pedido(t_datos_para_guardar* datos_para_bloques)
{
	t_pedido_sind* pedido_sind = desglosar_pedido (datos_para_bloques->data);

	bool op_ok = string_confirmar_pedido(pedido_sind);
		if (!op_ok)
			return false;

	datos_para_bloques->data = armar_string_pedido(pedido_sind);
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
