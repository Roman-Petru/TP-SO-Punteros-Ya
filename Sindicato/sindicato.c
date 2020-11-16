#include "sindicato.h"

/*#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
*/

t_servidor_red* servidor;
t_log* logger;
t_config* config;

void inicializar_sindicato()
{
	logger = log_create("sindicato.log", "SINDICATO", true, LOG_LEVEL_INFO);
	config = config_create("sindicato.config");

	serializacion_inicializar();
	servidor = servidor_crear("127.0.0.1", config_get_string_value(config, "PUERTO_ESCUCHA"));
	//cargar_interfaz();

	inicializar_metadata();
	inicializar_bloques();
	inicializar_archivos();

}


int main()
{
	inicializar_sindicato();
	int sumar_extra = 0;

    if (3%4 != 0)
    	sumar_extra = 1;

	int i = 3/4 + sumar_extra;

	printf("%d", i);

	sumar_extra = 0;

    if (8%4 != 0)
    	sumar_extra = 1;

	int j = 8/4 + sumar_extra;
	printf("%d", j);
}


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
