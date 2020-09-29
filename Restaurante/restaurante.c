#include "restaurante.h"

int multiprocesamiento_restaurante;
int numero_afinidades;
int cantidad_hornos;

t_list* lista_afinidades;


void inicializar_restaurante()
{

	//obtener_metadata();
	multiprocesamiento_restaurante = 5;

	lista_afinidades = list_create();
	list_add(lista_afinidades, "milanesas");
	list_add(lista_afinidades, "pure");
	numero_afinidades = list_size(lista_afinidades);
	const char *platos[3];
	platos[0] = "milanesas";
	platos[1] = "pure";
	platos[2] = "ensalada";
	cantidad_hornos = 3;

	//obtener recetas
	//termina metadata


	cola_Resto_NEW = list_create();
	cola_de_cola_Resto_READY = list_create();

	for (int i=0; i<=numero_afinidades;i++){
	t_list* cola_Resto_READY = list_create();
//	printf("%s%d",list_get(lista_afinidades, i), i);
	}

	cola_Resto_BLOCKED = list_create();
	cola_Resto_EXEC = list_create();
	cola_Resto_EXIT = list_create();

	inicializar_diccionario_colas();
}


int main()

{
	inicializar_restaurante();

	//while (1)
	{


	}

}
