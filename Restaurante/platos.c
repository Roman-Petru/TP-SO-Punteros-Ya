#include "platos.h"
#include "restaurante.h"

t_dictionary* diccionario_recetas;


t_platos_PCB* crear_plato(char* plato, int id_pedido)
{
	t_platos_PCB* nuevo_plato = malloc(sizeof(t_platos_PCB));

	nuevo_plato->id_PCB = id_PCB;
	id_PCB++;

	nuevo_plato->id_pedido = id_pedido;
	nuevo_plato->nombre_plato = plato;
	nuevo_plato->estado_pcb = NEW;

	nuevo_plato->ciclos_ejecutandose = 0;
	nuevo_plato->hubo_cambio_operacion = false;

	nuevo_plato->pasos_receta_faltantes = list_duplicate(dictionary_get(diccionario_recetas, plato));
	
	t_paso* paso = list_get(nuevo_plato->pasos_receta_faltantes, 0);
	nuevo_plato->ciclos_restantes_paso_actual = paso->ciclos;


	sem_init(&(nuevo_plato->mutex), 0, 0);
	pthread_create(&(nuevo_plato->hilo), NULL, (void*) &ciclo_plato, nuevo_plato);
	pthread_detach(nuevo_plato->hilo);

	list_add(cola_Resto_NEW, nuevo_plato);
	log_info(logger_resto, "Se creo el plato de %s con PCB %d", nuevo_plato->nombre_plato, nuevo_plato->id_PCB);

	if (strcmp(paso->operacion, "Hornear") == 0)
	{
		cambiar_estado_a(nuevo_plato, BLOCKED);
		list_add(cola_Hornos_READY, nuevo_plato);
	} else if (strcmp(paso->operacion, "Reposar") == 0)
	{
		cambiar_estado_a(nuevo_plato, BLOCKED);
		logear_inicio_operacion(nuevo_plato);
	} else
	{
		cambiar_estado_a(nuevo_plato, READY);
	}


	return nuevo_plato;

}




void ciclo_plato(t_platos_PCB* plato)
{
	while(1)
	{
	sem_wait(&(plato->mutex));
	//((t_accion) dictionary_int_get(diccionario_acciones, pedido->instruccion_a_realizar))(pedido); avanzar paso?
	avanzar_paso_receta(plato);
	sem_post(&(semaforo_resto));
	}
}


void avanzar_paso_receta (t_platos_PCB* plato)
{

	plato->ciclos_restantes_paso_actual--;

	if(plato->estado_pcb == EXEC)
		plato->ciclos_ejecutandose++;

	log_info(logger_resto, "Se avanzo un paso en el plato con PCB %d, ahora quedan %d pasos en la operacion que se encuentra", plato->id_PCB, plato->ciclos_restantes_paso_actual);
}

void cambiar_paso_de_ser_necesario(t_platos_PCB* plato)
{
	if (plato->ciclos_restantes_paso_actual == 0)
	{
		log_info(logger_resto, "Se finalizo con la operacion de %s del plato de %s con PCB %d", (((t_paso*) list_get(plato->pasos_receta_faltantes, 0))->operacion), plato->nombre_plato,plato->id_PCB);
		plato->hubo_cambio_operacion = true;
		list_remove(plato->pasos_receta_faltantes, 0);
		if (!list_is_empty(plato->pasos_receta_faltantes))
		{
			t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
			plato->ciclos_restantes_paso_actual = paso->ciclos;
			//log_info(logger_resto, "Se avanzo en la operacion, el plato con PCB %d ahora se va a %s", plato->id_PCB, paso->operacion);
		}
	}

}

void terminar_plato (t_platos_PCB* plato)
{

	cambiar_estado_a(plato, EXIT);
	log_info(logger_resto, "Se finalizo %s, correspondiente al PCB %d", plato->nombre_plato, plato->id_PCB);
	pthread_cancel(plato->hilo);
}


void logear_inicio_operacion(t_platos_PCB* plato)
{
	log_info(logger_resto, "Se comenzo la operacion de %s del plato de %s con PCB %d", (((t_paso*) list_get(plato->pasos_receta_faltantes, 0))->operacion), plato->nombre_plato, plato->id_PCB);
}


void inicializar_diccionario_recetas()
{
	//obtener recetas del SINDICATO

	t_receta* milanesa = malloc(sizeof(t_receta));
	milanesa->nombre_plato = "milanesa";
	milanesa->pasos_receta = list_create();
	t_paso* paso1 = malloc(sizeof(t_paso));
	paso1->operacion = "Amasar";
	paso1->ciclos = 4;
	list_add(milanesa->pasos_receta, paso1);
	t_paso* paso2 = malloc(sizeof(t_paso));
	paso2->operacion = "Panear";
	paso2->ciclos = 2;
	list_add(milanesa->pasos_receta, paso2);
	t_paso* paso3 = malloc(sizeof(t_paso));
	paso3->operacion = "Cuchi";
	paso3->ciclos = 4;
	list_add(milanesa->pasos_receta, paso3);

	diccionario_recetas = dictionary_create();
	dictionary_put(diccionario_recetas, milanesa->nombre_plato, milanesa->pasos_receta);

	free(milanesa);

	t_receta* pure = malloc(sizeof(t_receta));
	pure->nombre_plato = "pure";
	pure->pasos_receta = list_create();
	t_paso* paso4 = malloc(sizeof(t_paso));
	paso4->operacion = "trocear";
	paso4->ciclos = 2;
	list_add(pure->pasos_receta, paso4);
	t_paso* paso5 = malloc(sizeof(t_paso));
	paso5->operacion = "Calentar";
	paso5->ciclos = 2;
	list_add(pure->pasos_receta, paso5);
	t_paso* paso7 = malloc(sizeof(t_paso));
	paso7->operacion = "Reposar";
	paso7->ciclos = 2;
	list_add(pure->pasos_receta, paso7);

	dictionary_put(diccionario_recetas, pure->nombre_plato, pure->pasos_receta);
	free(pure);

	t_receta* ensalada = malloc(sizeof(t_receta));
	ensalada->nombre_plato = "ensalada";
	ensalada->pasos_receta = list_create();
	t_paso* paso8 = malloc(sizeof(t_paso));
	paso8->operacion = "Reposar";
	paso8->ciclos = 3;
	list_add(ensalada->pasos_receta, paso8);
	t_paso* paso9 = malloc(sizeof(t_paso));
	paso9->operacion = "PAPEAR";
	paso9->ciclos = 4;
	list_add(ensalada->pasos_receta, paso9);
	t_paso* paso10 = malloc(sizeof(t_paso));
	paso10->operacion = "Hornear";
	paso10->ciclos = 1;
	list_add(ensalada->pasos_receta, paso10);

	dictionary_put(diccionario_recetas, ensalada->nombre_plato, ensalada->pasos_receta);
	free(ensalada);
}
