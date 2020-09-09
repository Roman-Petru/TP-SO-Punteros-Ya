#include "repartidores.h"




t_list* inicializar_repartidores()
{
	t_list* lista_repartidores_libres = list_create();
	t_config* config = leer_config("app.config");
	char** repartidores = config_get_array_value(config, "REPARTIDORES");
	char** tiempos_de_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	char** frecuencias_de_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");

	int i = 0;

			while (repartidores[i] != NULL)
			{
				t_repartidor* repartidor = malloc(sizeof(t_repartidor));

				//SEPARO POSICIONES X E Y DE REPARTIDOR
				char* token;
				token = strtok(repartidores[i], "|");
				repartidor->posicion_x = atoi (token);
				token = strtok(NULL, "|");
				repartidor->posicion_y = atoi (token);



				repartidor->frecuencia_descanso = atoi (frecuencias_de_descanso[i]);
				repartidor->tiempo_descanso = atoi (tiempos_de_descanso[i]);

				list_add(lista_repartidores_libres, repartidor);
				free(repartidor);

				i = i+1;

			}
	return lista_repartidores_libres;
}



t_repartidor* mover_repartidor_hacia(t_repartidor* repartidor, int hacia_posicion_x, int hacia_posicion_y)
{
	if (repartidor->posicion_x > hacia_posicion_x){
		repartidor->posicion_x = repartidor->posicion_x - 1;
	}else if (repartidor->posicion_x < hacia_posicion_x){
		repartidor->posicion_x = repartidor->posicion_x + 1;
	}else if (repartidor->posicion_y < hacia_posicion_y){
		repartidor->posicion_y = repartidor->posicion_y + 1;
	}else if (repartidor->posicion_y > hacia_posicion_y){
		repartidor->posicion_y = repartidor->posicion_y - 1;
	}
	return repartidor;
}

int calcular_distancia (int posicion_x, int posicion_y, int hacia_posicion_x, int hacia_posicion_y)
{
	int distancia = sqrt((hacia_posicion_x - posicion_x) * (hacia_posicion_x - posicion_x) + (hacia_posicion_y - posicion_y) * (hacia_posicion_y - posicion_y));
	return distancia;
}


