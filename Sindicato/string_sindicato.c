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

/*
bool mod_string_guardar_plato(t_datos_para_guardar* datos_para_bloques, t_guardar_plato* datos_a_guardar)
{

	char** aux = string_n_split(datos_para_bloques->data, 5, "\n");

	char* estado = (strchr(aux[0], '='))+1;
	if (strcmp(estado, "Pendiente") != 0)
		{log_info(logger, "No se pudo agregar plato porque el pedido no estaba en estado pendiente");
		free(estado);
		return false;	}
	free(estado);

	char* nuevo_string = string_new();
	string_append(&nuevo_string, aux[0]);

	char* aux_platos = (strchr(aux[1], '='))+1;
	char* platos = remover_corchetes(aux_platos);

	bool encontro_plato = false;
	int posicion_plato = 0;

	string_append(&nuevo_string, "\nLISTA_PLATOS=[");

		if (string_is_empty(platos))
			{string_append(&nuevo_string, datos_a_guardar->comida);
			}
		else
			{
			char** array_platos = string_split(platos, ',');
			for (; array_platos[posicion_plato] != NULL && !encontro_plato; posicion_plato++)
			{
				if(strcmp(array_platos[posicion_plato], datos_a_guardar->comida))
					{encontro_plato = true;}
			}

			if (!encontro_plato)
				{string_append(&nuevo_string, platos); string_append(&nuevo_string, ",");
				string_append(&nuevo_string, datos_a_guardar->comida);
			} else 	{
				string_append(&nuevo_string, platos); }
			}
	string_append(&nuevo_string, "]\nCANTIDAD_PLATOS=[");


	char* aux_cant_platos = (strchr(aux[2], '='))+1;
	char* cant_platos = remover_corchetes(aux_platos);

	char* aux_cantidad = string_itoa(datos_a_guardar->cantidad);

	if (string_is_empty(cant_platos))
		{
		string_append(&nuevo_string, aux_cantidad);
		} else if (!encontro_plato){
		string_append(&nuevo_string, ",");
		string_append(&nuevo_string, aux_cantidad);
		} else
		char** array_cant_platos = string_split(cant_platos, ',');



	 free(aux_cantidad);



	 return true;
	 //if (!string_contains(platos, datos_a_guardar->comida))
}*/

t_pedido_sind* desglosar_pedido (char* pedido_en_string)
{
	t_pedido_sind* pedido_sind = malloc(sizeof(t_pedido_sind));
	pedido_sind->platos = list_create();


	char** aux = string_n_split(pedido_en_string, 5, "\n");
	pedido_sind->estado = (strchr(aux[0], '='))+1;

	free(aux[0]);

	char* aux_platos = (strchr(aux[1], '='))+1;
	char* platos = remover_corchetes(aux_platos);

	char* aux_cant_platos = (strchr(aux[2], '='))+1;
	char* cant_platos = remover_corchetes(aux_cant_platos);

	char* aux_cant_platos_lista = (strchr(aux[3], '='))+1;
	char* cant_platos_lista = remover_corchetes(aux_cant_platos_lista);



	if (!string_is_empty(platos))
		{char** array_platos = string_split(platos, ",");
		char** array_cant_platos = string_split(cant_platos, ",");
		char** array_cant_platos_lista = string_split(cant_platos_lista, ",");

		for (int i=0; array_platos[i] != NULL;i++)
			{
			uint32_t cant_total = strtol(array_cant_platos[i], NULL, 10);
			uint32_t cant_lista = strtol(array_cant_platos_lista[i], NULL, 10);
			t_datos_estado_comida* estado_comida = crear_datos_estado_comida(array_platos[i], cant_total, cant_lista);

			//free(array_platos[i]);
			free(array_cant_platos[i]);
			free(array_cant_platos_lista[i]);

			list_add(pedido_sind->platos, estado_comida);
			}

		free(array_platos); free(array_cant_platos); free(array_cant_platos_lista);
		}

	//free(platos)

	char* aux_precio = (strchr(aux[4], '='))+1;
	pedido_sind->precio = strtol(aux_precio, NULL, 10);
	//free(aux_precio); rompe

	return pedido_sind;
}


bool guardar_plato(t_pedido_sind* pedido_sind, t_guardar_plato* datos_a_guardar)
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

	bool op_ok = guardar_plato(pedido_sind, datos_a_guardar);
		if (!op_ok)
			return false;

	datos_para_bloques->data = armar_string_pedido(pedido_sind);
	return true;

}
