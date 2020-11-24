#include "platos.h"
#include "restaurante.h"




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

	//log_info(logger_resto, "Se avanzo un paso en el plato con PCB %d, ahora quedan %d pasos en la operacion que se encuentra", plato->id_PCB, plato->ciclos_restantes_paso_actual);
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

	t_guardar_plato* datos = crear_datos_guardar_plato(plato->id_pedido, 1, plato->nombre_plato, nombre_restaurante);
	sem_wait (&(mutex_cliente));
	cliente_enviar_mensaje(cliente_sind, PLATO_LISTO, datos);
	if (app_activada)
		cliente_enviar_mensaje(cliente_app, PLATO_LISTO, datos);
	sem_post (&(mutex_cliente));

	bool buscar_pedido (void* estado_platos)
	{t_platos_listos* platos_listos = estado_platos;
	return platos_listos->id_pedido == plato->id_pedido;}

	pthread_mutex_lock(&mutex_pedidos);
	t_platos_listos* platos_listos = list_find(lista_pedidos, &buscar_pedido);
	platos_listos->platos_listos++;
	bool terminar_pedido = (platos_listos->platos_listos==platos_listos->platos_total);
	pthread_mutex_unlock(&mutex_pedidos);

	if (terminar_pedido)
		{cliente_enviar_mensaje(cliente_sind, TERMINAR_PEDIDO,  crear_datos_pedido(plato->id_pedido, nombre_restaurante));}
		//TODO MANDAR AL CLIENTE SI EL CLIENTE MANDO CONFIRMAR PEDIDO(no es prioridad)
	//TODO dstruir PCB

}


void logear_inicio_operacion(t_platos_PCB* plato)
{
	log_info(logger_resto, "Se comenzo la operacion de %s del plato de %s con PCB %d", (((t_paso*) list_get(plato->pasos_receta_faltantes, 0))->operacion), plato->nombre_plato, plato->id_PCB);
}

