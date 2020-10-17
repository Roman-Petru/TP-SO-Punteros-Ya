#include "restaurante.h"

t_config* config_resto;
int id_PCB;
int cantidad_hornos;
int quantum;

t_log* logger_resto;

t_list* cola_Resto_NEW;
t_list* cola_de_cola_Resto_READY;
t_list* cola_Resto_BLOCKED;
t_list* cola_de_cola_Resto_EXEC;
t_list* cola_Resto_EXIT;

t_list* cola_Hornos_READY;
t_list* cola_Hornos_EXEC;

t_list* lista_afinidades;

sem_t semaforo_resto;

void inicializar_restaurante()
{

	logger_resto = log_create("resto.log", "RESTAURANTE", true, LOG_LEVEL_INFO);

	id_PCB = 0;
	config_resto = config_create("restaurante.config");
	quantum = config_get_int_value(config_resto, "QUANTUM");
	//obtener_metadata();

	lista_afinidades = list_create();
	t_afinidad* afinidad1 = malloc(sizeof(t_afinidad));
	afinidad1->plato = "milanesa";
	afinidad1->cantidad_cocineros = 3;
	list_add(lista_afinidades, afinidad1);
	t_afinidad* afinidad2 = malloc(sizeof(t_afinidad));
	afinidad2->plato = "pure";
	afinidad2->cantidad_cocineros = 1;
	list_add(lista_afinidades, afinidad2);
	t_afinidad* afinidad3 = malloc(sizeof(t_afinidad));
	afinidad3->plato = "general";
	afinidad3->cantidad_cocineros = 3;
	list_add(lista_afinidades, afinidad3);
/*
	const char *platos[3];
	platos[0] = "milanesa";
	platos[1] = "pure";
	platos[2] = "ensalada";
*/

	cantidad_hornos = 3;

	//obtener recetas
	//termina metadata

	sem_init (&(semaforo_resto), 0, 0);

	cola_Resto_NEW = list_create();
	cola_Resto_BLOCKED = list_create();

	cola_de_cola_Resto_READY = list_create();
	cola_de_cola_Resto_EXEC = list_create();
	for (int i=0; i<list_size(lista_afinidades);i++){
		t_list* cola_Resto_READY;
		cola_Resto_READY = list_create();
		list_add(cola_de_cola_Resto_READY, cola_Resto_READY);
		//free(cola_Resto_READY);
		t_list* cola_Resto_EXIT;
		cola_Resto_EXIT = list_create();
		list_add(cola_de_cola_Resto_EXEC, cola_Resto_EXIT);
		//free(cola_Resto_EXIT);
	}



	cola_Resto_EXIT = list_create();
	cola_Hornos_READY = list_create();
	cola_Hornos_EXEC = list_create();

	inicializar_diccionario_recetas();
	inicializar_diccionario_colas();
}


int main()

{
	inicializar_restaurante();
	crear_plato("milanesa", 54);
	crear_plato("milanesa", 65);
	crear_plato("milanesa", 80);
	crear_plato("ensalada", 100);
	crear_plato("ensalada", 120);
	crear_plato("ensalada", 130);
	crear_plato("pure", 90);

	while (1)
	{
		planificar_corto_plazo();
		ejecutar_ciclo();

	}

}
