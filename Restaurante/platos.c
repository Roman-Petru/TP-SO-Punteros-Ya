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

	nuevo_plato->pasos_receta_faltantes = malloc (sizeof(t_receta));
	nuevo_plato->pasos_receta_faltantes = dictionary_get(diccionario_recetas, plato); //ENCONTRE EL BUG, COMPARTEN LA RECETA, FALTA ARREGLARLO, 
	//PROBAR ALGUNA FORMA DE DUPLICAR LA LISTA
	
	t_paso* paso = list_get(nuevo_plato->pasos_receta_faltantes, 0);
	nuevo_plato->ciclos_restantes_paso_actual = paso->ciclos;


	sem_init(&(nuevo_plato->mutex), 0, 0);
	pthread_create(&(nuevo_plato->hilo), NULL, (void*) &ciclo_plato, nuevo_plato);
	//pthread_detach(&(nuevo_plato->hilo));

	list_add(cola_Resto_NEW, nuevo_plato);
	cambiar_estado_a(nuevo_plato, READY);

	return nuevo_plato;

}




void ciclo_plato(t_platos_PCB* plato)
{
	while(1)
	{
	sem_wait(&(plato->mutex));
	//((t_accion) dictionary_int_get(diccionario_acciones, pedido->instruccion_a_realizar))(pedido); avanzar paso?
	avanzar_paso_receta(plato);
	plato->ciclos_ejecutandose++;
	sem_post(&(semaforo_resto));
	}
}


bool avanzar_paso_receta (t_platos_PCB* plato)
{
	plato->ciclos_restantes_paso_actual--;

	log_info(logger_resto, "Se avanzo un paso en el plato con PCB %d, ahora quedan %d pasos en la operacion que se encuentra", plato->id_PCB, plato->ciclos_restantes_paso_actual);

	if (plato->ciclos_restantes_paso_actual == 0)
	{
		list_remove(plato->pasos_receta_faltantes, 0);
		if (!list_is_empty(plato->pasos_receta_faltantes))
		{
			t_paso* paso = list_get(plato->pasos_receta_faltantes, 0);
			plato->ciclos_restantes_paso_actual = paso->ciclos;
			log_info(logger_resto, "Se avanzo en la operacion, el plato con PCB %d ahora se va a %s", plato->id_PCB, paso->operacion);
			plato->hubo_cambio_operacion = true;
			return true;
		}
	}
	return false;
}

void inicializar_diccionario_recetas()
{
	//obtener recetas del SINDICATO

	t_receta* milanesa = malloc(sizeof(t_receta));
	milanesa->nombre_plato = "milanesa";
	milanesa->pasos_receta = list_create();
	t_paso* paso1 = malloc(sizeof(t_paso));
	paso1->operacion = "trocear";
	paso1->ciclos = 2;
	list_add(milanesa->pasos_receta, paso1);
	t_paso* paso2 = malloc(sizeof(t_paso));
	paso2->operacion = "REPOSAR";
	paso2->ciclos = 2;
	list_add(milanesa->pasos_receta, paso2);
	t_paso* paso3 = malloc(sizeof(t_paso));
	paso3->operacion = "HORNEAR";
	paso3->ciclos = 2;
	list_add(milanesa->pasos_receta, paso3);

	diccionario_recetas = dictionary_create();
	dictionary_put(diccionario_recetas, milanesa->nombre_plato, milanesa->pasos_receta);



	t_receta* pure = malloc(sizeof(t_receta));
	pure->nombre_plato = "pure";
	pure->pasos_receta = list_create();
	t_paso* paso4 = malloc(sizeof(t_paso));
	paso4->operacion = "trocear";
	paso4->ciclos = 6;
	list_add(pure->pasos_receta, paso4);
	t_paso* paso5 = malloc(sizeof(t_paso));
	paso5->operacion = "REPOSAR";
	paso5->ciclos = 2;
	list_add(pure->pasos_receta, paso5);
	t_paso* paso7 = malloc(sizeof(t_paso));
	paso7->operacion = "HORNEAR";
	paso7->ciclos = 4;
	list_add(pure->pasos_receta, paso7);


	dictionary_put(diccionario_recetas, pure->nombre_plato, pure->pasos_receta);
}
