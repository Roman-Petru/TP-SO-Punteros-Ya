void cargar_interfaz()
{


}

void guardar_Pedido(int idPedido, char* restaurante)
{
	int *p = malloc(sizeof (int));
	int *r = malloc(sizeof (int));
	*p = idPedido
	*r = restaurante

	return respuesta_crear(GUARDAR_PEDIDO_RESPUESTA,(void*) true,false);
}

void guardar_Plato(int idPedido,char* restaurante, char* comida, int cantidad )
{
	int *p = malloc(sizeof (int));
	int *r = malloc(sizeof (int));
	int *co = malloc(sizeof (int));
	int *c = malloc(sizeof (int));

	*p = idPedido
	*r = restaurante
	*co = comida
	*c = cantidad

	return respuesta_crear(GUARDAR_PLATO_RESPUESTA,(void*) true,false);
}


