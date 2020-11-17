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

	t_list* bloques_siguientes = list_create();
	guardar_data_en_bloques(datos_archivo_a_bloques, bloque_inicial, bloques_siguientes);

	return true;

}

