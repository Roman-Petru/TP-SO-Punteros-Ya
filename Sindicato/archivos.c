#include "archivos.h"


void inicializar_archivos()
{
	char* files = string_new();
	string_append(&files, raiz->nombre);
	string_append(&files,"/Files");
	t_nodo_arbol* nodo_files = crearNodo("/Files");
	agregarNodo(raiz,nodo_files);

	if (!existe_directorio(files))
		mkdir(files, 0700);

	char* restaurantes = string_new();
	string_append(&restaurantes, files);
	string_append(&restaurantes,"/Restaurantes");
	agregarNodo(nodo_files ,crearNodo("/Restaurantes"));

	if (!existe_directorio(restaurantes))
		mkdir(restaurantes, 0700);


	char* recetas = string_new();
	string_append(&recetas, files);
	string_append(&recetas,"/Recetas");
	agregarNodo(nodo_files ,crearNodo("/Recetas"));

	if (!existe_directorio(recetas))
		mkdir(recetas, 0700);

	free(restaurantes);
	free(recetas);
	free(files);

}


bool restaurante_existe(char* restaurante)
{

	char* nodo = obtener_nodo_restaurantes();
	//obtener nodo?

	DIR* dir = opendir(nodo);
	struct dirent* entry;
	bool encontro = false;
	if (dir == NULL) {
		log_error(logger,"No se pudo abrir correctamente el directorio Restaurantes");
	} else {
		while((entry = readdir(dir)) && !encontro) {
		//	log_info(logger,"prueba dir name: %s", entry->d_name);
			if (string_equals_ignore_case(entry->d_name, restaurante))
				encontro = true;
		}}

	free(nodo);
	closedir(dir);

	return encontro;
}

static char* armar_string_arch_pedido(int id_pedido)
{
char* nombre_arch_pedido = string_new();
char* numero_pedido = string_itoa(id_pedido);
string_append(&nombre_arch_pedido, "Pedido");
string_append(&nombre_arch_pedido, numero_pedido);
string_append(&nombre_arch_pedido, ".AFIP");

free(numero_pedido);
return nombre_arch_pedido;
}

bool pedido_existe(char* nodo_resto, int id_pedido)
{
	char* nombre_arch_pedido = armar_string_arch_pedido(id_pedido);
	DIR* dir = opendir(nodo_resto);
	struct dirent* entry;
	bool encontro = false;

	if (dir == NULL) {
		log_error(logger,"No se pudo abrir correctamente el directorio");
	} else {
		while((entry = readdir(dir)) && !encontro) {

			if (string_equals_ignore_case(entry->d_name, nombre_arch_pedido))
				encontro = true;

		}}

	free(nombre_arch_pedido);
	closedir(dir);
	return encontro;
}

char* crear_string_archivo_info(int size_arch, uint32_t bloque_inicial)
{
	char* datos_archivo_info = string_new();
	string_append(&datos_archivo_info, "SIZE=");
	char* size = string_itoa(size_arch);
	string_append(&datos_archivo_info, size);
	string_append(&datos_archivo_info, "\nINITIAL_BLOCK=");
	char* string_bloque = string_itoa(bloque_inicial);
	string_append(&datos_archivo_info, string_bloque);

	free (size);
	return datos_archivo_info;
}

bool crear_archivo_pedido(char* nodo_resto, int id_pedido)
{
	int bloque_inicial = reservar_bloque();
	if (bloque_inicial < 0)
		{log_info(logger, "No se pudo crear archivo de pedido por falta de espacio en disco"); return false;}

	char* datos_archivo_a_bloques = "ESTADO_PEDIDO=Pendiente\nLISTA_PLATOS=[]\nCANTIDAD_PLATOS=[]\nCANTIDAD_LISTA=[]\nPRECIO_TOTAL=0";

	char* datos_archivo_info = crear_string_archivo_info(strlen(datos_archivo_a_bloques), bloque_inicial);

	char* nombre_arch_pedido = string_new();
	string_append(&nombre_arch_pedido, nodo_resto);
	string_append(&nombre_arch_pedido, "/");
	string_append(&nombre_arch_pedido, armar_string_arch_pedido(id_pedido));

	int fd = open(nombre_arch_pedido, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
	posix_fallocate(fd, 0, strlen(datos_archivo_info));
	char* archivo_en_memoria = mmap(NULL, strlen(datos_archivo_info), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(archivo_en_memoria, datos_archivo_info, strlen(datos_archivo_info));

	munmap(archivo_en_memoria, strlen(datos_archivo_info));
	close(fd);
	free(datos_archivo_info); free(nombre_arch_pedido);


	t_datos_para_guardar* datos = malloc(sizeof(t_datos_para_guardar));
	datos->data = datos_archivo_a_bloques;
	datos->bloque_inicial = bloque_inicial;
	datos->bloques_siguientes = list_create();

	guardar_data_en_bloques(datos, nombre_arch_pedido);

	return true;
}

uint32_t obtener_bloque_inicial(char* info_bloque)
{
	char* aux = strstr(info_bloque, "CK=");
	aux = aux+3;
	return strtol(aux, NULL, 10);
}

void modificar_tamanio_real(char* path_archivo, int tamanio)
{
	int fd = open(path_archivo, O_RDWR, S_IRWXU | S_IRWXG);

	struct stat sb;
	fstat(fd, &sb);

	char* archivo_en_memoria = mmap(NULL, sb.st_size+3, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	uint32_t bloque = obtener_bloque_inicial(archivo_en_memoria);
	char* datos_archivo_info = crear_string_archivo_info(tamanio, bloque);

	posix_fallocate(fd, 0, strlen(datos_archivo_info));
	memcpy(archivo_en_memoria, datos_archivo_info, strlen(datos_archivo_info));
	free(datos_archivo_info);
	munmap(archivo_en_memoria, sb.st_size+3);
	close(fd);
}


void crear_receta(char* data)
{
	int bloque_inicial = reservar_bloque();
	if (bloque_inicial < 0)
		{log_info(logger, "No se pudo crear archivo de pedido por falta de espacio en disco"); return;}

	char** aux = string_n_split(data, 3, " ");

	char* datos_receta = string_new();
	string_append(&datos_receta, "PASOS=");
	string_append(&datos_receta, aux[1]);
	string_append(&datos_receta, "\nTIEMPO_PASOS=");
	string_append(&datos_receta, aux[2]);

	char* datos_archivo_info = crear_string_archivo_info(strlen(datos_receta), bloque_inicial);

	char* nombre_arch_recetas = string_new();
	string_append(&nombre_arch_recetas, obtener_nodo_recetas());
	string_append(&nombre_arch_recetas, "/");
	string_append(&nombre_arch_recetas, aux[0]);
	string_append(&nombre_arch_recetas, ".AFIP");


	int fd = open(nombre_arch_recetas, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
	posix_fallocate(fd, 0, strlen(datos_archivo_info));
	char* archivo_en_memoria = mmap(NULL, strlen(datos_archivo_info), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(archivo_en_memoria, datos_archivo_info, strlen(datos_archivo_info));

	munmap(archivo_en_memoria, strlen(datos_archivo_info));
	close(fd);
	free(datos_archivo_info); free(nombre_arch_recetas);
	free(aux[0]); free(aux[1]); free(aux[2]); free(aux);

	t_datos_para_guardar* datos = malloc(sizeof(t_datos_para_guardar));
	datos->data = datos_receta;
	datos->bloque_inicial = bloque_inicial;
	datos->bloques_siguientes = list_create();

	guardar_data_en_bloques(datos, nombre_arch_recetas);
}


void crear_restaurante (char* data)
{
	int bloque_inicial = reservar_bloque();
	if (bloque_inicial < 0)
		{log_info(logger, "No se pudo crear archivo de pedido por falta de espacio en disco"); return;}

	char** aux = string_n_split(data, 7, " ");

	char* datos_restaurante = string_new();
	string_append(&datos_restaurante, "CANTIDAD_COCINEROS=");
	string_append(&datos_restaurante, aux[1]);
	string_append(&datos_restaurante, "\nPOSICION=");
	string_append(&datos_restaurante, aux[2]);
	string_append(&datos_restaurante, "\nAFINIDAD_COCINEROS=");
	string_append(&datos_restaurante, aux[3]);
	string_append(&datos_restaurante, "\nPLATOS=");
	string_append(&datos_restaurante, aux[4]);
	string_append(&datos_restaurante, "\nPRECIO_PLATOS=");
	string_append(&datos_restaurante, aux[5]);
	string_append(&datos_restaurante, "\nCANTIDAD_HORNOS=");
	string_append(&datos_restaurante, aux[6]);

	char* datos_archivo_info = crear_string_archivo_info(strlen(datos_restaurante), bloque_inicial);

	char* nombre_arch_recestaurante = string_new();
	string_append(&nombre_arch_recestaurante, obtener_nodo_restaurante_especifico(aux[0]));
	mkdir(nombre_arch_recestaurante, 0700);
	string_append(&nombre_arch_recestaurante, "/Info.AFIP");


	int fd = open(nombre_arch_recestaurante, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
	posix_fallocate(fd, 0, strlen(datos_archivo_info));
	char* archivo_en_memoria = mmap(NULL, strlen(datos_archivo_info), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(archivo_en_memoria, datos_archivo_info, strlen(datos_archivo_info));

	munmap(archivo_en_memoria, strlen(datos_archivo_info));
	close(fd);
	free(datos_archivo_info); free(nombre_arch_recestaurante);
	for (int i=0; i<7;i++)
		free(aux[i]);
	free(aux);


	t_datos_para_guardar* datos = malloc(sizeof(t_datos_para_guardar));
	datos->data = datos_restaurante;
	datos->bloque_inicial = bloque_inicial;
	datos->bloques_siguientes = list_create();

	guardar_data_en_bloques(datos, nombre_arch_recestaurante);
}
