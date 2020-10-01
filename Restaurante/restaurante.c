#include "restaurante.h"

int multiprocesamiento_restaurante;
int cantidad_hornos;

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

	//obtener_metadata();
	multiprocesamiento_restaurante = 5;

	lista_afinidades = list_create();
	t_afinidad* afinidad1 = malloc(sizeof(t_afinidad));
	afinidad1->plato = "milanesas";
	afinidad1->cantidad_cocineros = 1;
	list_add(lista_afinidades, afinidad1);
	t_afinidad* afinidad2 = malloc(sizeof(t_afinidad));
	afinidad2->plato = "pure";
	afinidad2->cantidad_cocineros = 1;
	list_add(lista_afinidades, afinidad2);
	t_afinidad* afinidad3 = malloc(sizeof(t_afinidad));
	afinidad3->plato = "general";
	afinidad3->cantidad_cocineros = 3;
	list_add(lista_afinidades, afinidad3);

	const char *platos[3];
	platos[0] = "milanesas";
	platos[1] = "pure";
	platos[2] = "ensalada";


	cantidad_hornos = 3;

	//obtener recetas
	//termina metadata

	sem_init (&(semaforo_resto), 0, 0);

	cola_Resto_NEW = list_create();
	cola_Resto_BLOCKED = list_create();

	cola_de_cola_Resto_READY = list_create();
	cola_de_cola_Resto_EXEC = list_create();
	for (int i=0; i<list_size(lista_afinidades);i++){
		t_list* cola_Resto_READY = malloc (sizeof(t_list));
		cola_Resto_READY = list_create();
		list_add(cola_de_cola_Resto_READY, cola_Resto_READY);
		t_list* cola_Resto_EXIT = malloc (sizeof(t_list));
		cola_Resto_EXIT = list_create();
		list_add(cola_de_cola_Resto_EXEC, cola_Resto_EXIT);
	}



	cola_Resto_EXIT = list_create();
	cola_Hornos_READY = list_create();
	cola_Hornos_EXEC = list_create();
	inicializar_diccionario_colas();
}


int main()

{
	inicializar_restaurante();

	//while (1)
	{
		planificar_corto_plazo();
		ejecutar_ciclo();

	}

}
