#include "app.h"




int main()
{

}



t_list* inicializar_repartidores()
{
	t_list* lista_repartidores = list_create();
	t_config* config = leer_config("app.config");
	char** repartidores = config_get_array_value(config, "REPARTIDORES");
	char** tiempos_de_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
	char** frecuencias_de_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");

	int i = 0;

			while (repartidores[i] != NULL)
			{
				t_repartidores* repartidor = malloc(sizeof(t_repartidores));

				//SEPARO POSICIONES X E Y DE REPARTIDOR
				char* token;
				token = strtok(repartidores[i], "|");
				repartidor->posicion_x = atoi (token);
				token = strtok(NULL, "|");
				repartidor->posicion_y = atoi (token);



				repartidor->frecuencia_descanso = atoi (frecuencias_de_descanso[i]);
				repartidor->tiempo_descanso = atoi (tiempos_de_descanso[i]);

				list_add(lista_repartidores, repartidor);
				free(repartidor);

				i = i+1;

			}
	return lista_repartidores;
}

