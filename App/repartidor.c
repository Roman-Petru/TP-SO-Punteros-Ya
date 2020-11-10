#include "repartidor.h"
#include "app.h"

t_list* repartidores_libres;

//========== REPARTIDOR ==========//
t_repartidor* repartidor_crear(t_posicion* posicion, int frecuencia_descanso, int tiempo_descanso)
{
	t_repartidor* repartidor = malloc(sizeof(t_repartidor));

	repartidor->posicion = posicion;
	repartidor->frecuencia_descanso = frecuencia_descanso;
	repartidor->tiempo_descanso = tiempo_descanso;

	repartidor->ciclos_viajando = 0;
	repartidor->ciclos_descansados = 0;

	repartidor->esta_cansado = false;

	return repartidor;
}

void repartidor_destruir(t_repartidor* repartidor)
{
	free(repartidor->posicion);
	free(repartidor);
}

void repartidor_descansar(t_repartidor* repartidor)
{
	repartidor->ciclos_descansados = repartidor->ciclos_descansados + 1;

	if(repartidor->ciclos_descansados < repartidor->tiempo_descanso)
		return;

	repartidor->esta_cansado = false;
	repartidor->ciclos_descansados = 0;
}

void repartidor_mover_hacia(t_repartidor* repartidor, t_posicion* destino)
{
	int x_actual = repartidor->posicion->x;
	int y_actual = repartidor->posicion->y;

	int x_destino = destino->x;
	int y_destino = destino->y;

	if(x_actual != x_destino) // Si no esta en x se acerca una pos a x
		repartidor->posicion->x = x_actual + (x_destino>x_actual ? 1 : -1 );

	else if(y_actual != y_destino) // Si no esta en y se acerca una pos a y
		repartidor->posicion->y = y_actual + (y_destino>y_actual ? 1 : -1 );

	repartidor->ciclos_viajando++;

	if(repartidor->ciclos_viajando < repartidor->frecuencia_descanso)
		return;

	repartidor->esta_cansado = true;
	repartidor->ciclos_viajando = 0;
}

//========== REPARTIDORES LIBRES ==========//
static t_repartidor*  repartidores_libres_remover(t_repartidor* repartidor)
{
	bool mismo_repartidor(t_repartidor* repartidor_comparar) { return repartidor_comparar == repartidor; }

	return list_remove_by_condition(repartidores_libres, (void*) &mismo_repartidor);
}

static void cargar_repartidor(char* posicion_str, char* frecuencia_de_descanso, char* tiempo_descanso)
{
	char** token = string_n_split(posicion_str, 2, "|");
	t_posicion* posicion = posicion_crear(strtol(token[0], NULL, 10), strtol(token[1], NULL, 10));

	free(token[0]);
	free(token[1]);
	free(token);

	t_repartidor* repartidor = repartidor_crear(posicion, strtol(frecuencia_de_descanso, NULL, 10), strtol(tiempo_descanso, NULL, 10));

	free(posicion_str);
	free(frecuencia_de_descanso);
	free(tiempo_descanso);

	list_add(repartidores_libres, repartidor);
}

void cargar_repartidores()
{
	repartidores_libres = list_create();

	char** repartidores = config_get_array_value(config, "REPARTIDORES");
	char** tiempos_de_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	char** frecuencias_de_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");

	for (int i=0; repartidores[i] != NULL; i++)
		cargar_repartidor(repartidores[i], frecuencias_de_descanso[i], tiempos_de_descanso[i]);

	free(repartidores);
	free(tiempos_de_descanso);
	free(frecuencias_de_descanso);
}

t_repartidor* repartidor_obtener_mas_cercano(t_posicion* destino)
{
	t_repartidor* repartidor_menor_distancia(t_repartidor* repartidor_A, t_repartidor* repartidor_B)
	{
		int distancia_A = posicion_distancia_entre(destino, repartidor_A->posicion);
		int distancia_B = posicion_distancia_entre(destino, repartidor_B->posicion);

		return distancia_A > distancia_B ? repartidor_B : repartidor_A;
	}

	t_repartidor* repartidor = list_fold(repartidores_libres, list_get(repartidores_libres, 0), (void*) &repartidor_menor_distancia);

	return repartidores_libres_remover(repartidor);
	/*
	int indice_repartidor_mas_cercano = 0;
	int indice_pivot = 0;

	t_repartidor* primer_repartidor = list_get(repartidores_libres, 0);
	int distancia_minima = posicion_distancia_entre(primer_repartidor->posicion, destino);


	void establecer_distancia_minima(t_repartidor* repartidor)
	{
		int distancia_repartidor_destino = posicion_distancia_entre(repartidor->posicion, destino);
		if (distancia_repartidor_destino < distancia_minima)
		{
			indice_repartidor_mas_cercano = indice_pivot;
			distancia_minima = distancia_repartidor_destino;
		}
		indice_pivot++;
	}
	list_iterate(repartidores_libres, (void*) &establecer_distancia_minima);

	return list_remove(repartidores_libres, indice_repartidor_mas_cercano);*/
}

