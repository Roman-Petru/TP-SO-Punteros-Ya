#include "sindicato.h"
#include <stdio.h>
/*
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
*/
#include <fcntl.h>

t_servidor_red* servidor;
t_log* logger;
t_config* config;

void inicializar_sindicato()
{
	logger = log_create("sindicato.log", "SINDICATO", true, LOG_LEVEL_INFO);
	config = config_create("sindicato.config");

	serializacion_inicializar();
	servidor = servidor_crear("127.0.0.1", config_get_string_value(config, "PUERTO_ESCUCHA"));
	cargar_interfaz();

	inicializar_metadata();
	inicializar_bloques();
	inicializar_archivos();
}


int main()
{
	inicializar_sindicato();

	log_info(logger, "Sindicato inicializado");

/*	char* prueba = "/home/utnso/Escritorio/afip/prueba.txt";
	char* texto = "BLABLABLA";

	int fd = open(prueba, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);

	posix_fallocate(fd, 0, strlen(texto));
	char* archivo_en_memoria2 = mmap(NULL, strlen(texto), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	perror("error: ");



	memcpy(archivo_en_memoria2, texto, strlen(texto));

	munmap(archivo_en_memoria2, strlen(texto));
	close(fd);*/
	while(true)
		{
		t_list* lista_numeros;
		lista_numeros = list_create();

		int* numero_bloque = malloc(sizeof(int));

		*numero_bloque = 5;


		list_add(lista_numeros, numero_bloque);

		int* numero = list_remove(lista_numeros, 0);

		log_info(logger, "%d", *numero);

		sleep(10000);}
}
/**/

//	int fd = open("./prueba.txt", O_RDWR, S_IRUSR | S_IWUSR);
//munmap(archivo_en_memoria, sb.st_size);
//char* archivo_en_memoria = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//close(fd);

	//struct stat sb;
	//fstat(fd, &sb);

/*void parse(const char* input){
  char *start, *end;
  unsigned count = 0;

  // the cast to (char*) is because i'm going to change the pointer, not because i'm going to change the value.
  start = end = (char*) input;

  while( (end = strchr(start, '\n')) ){
      printf("%d %.*s", count++, (int)(end - start + 1), start);
      start = end + 1;
}*/
