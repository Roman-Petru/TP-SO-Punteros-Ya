#include "restaurante.h"

t_config* config_resto;
int cantidad_hornos;
t_servidor_red* servidor;
t_log* logger_resto;

t_list* lista_afinidades;

void inicializar_restaurante()
{

	logger_resto = log_create("resto.log", "RESTAURANTE", true, LOG_LEVEL_INFO);
	config_resto = config_create("restaurante.config");

	serializacion_inicializar();
	servidor = servidor_crear("127.0.0.1", config_get_string_value(config_resto, "PUERTO_ESCUCHA"));

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

	//termina metadata

	inicializar_planificador();
	inicializar_interrupciones();
	inicializar_diccionario_recetas();

}

void terminar()
{
	serializacion_finalizar();
	servidor_destruir(servidor);
	log_info(logger_resto, "TERMINE");
	exit(0);
}


int main()

{
	inicializar_restaurante();

	//para test
	for (int i=0; i < 6; i++)
	{
	t_para_nuevo_plato* pure = malloc(sizeof(t_para_nuevo_plato));
	pure->nombre_plato = "pure";
	pure->id_pedido = i;
	agregar_interrupcion(NUEVO_PLATO, pure);
	}

	for (int i=7; i < 13; i++)
	{
	t_para_nuevo_plato* pure = malloc(sizeof(t_para_nuevo_plato));
	pure->nombre_plato = "milanesa";
	pure->id_pedido = i;
	agregar_interrupcion(NUEVO_PLATO, pure);
	}

	while (1)
	{
		planificar_corto_plazo();
		ejecutar_ciclo();
		ejecutar_interrupciones();
	}

}
