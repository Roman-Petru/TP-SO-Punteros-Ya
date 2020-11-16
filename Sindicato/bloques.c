#include "bloques.h"

static char* path_bloque(char* nombre) {
	char* path = string_new();
	string_append(&path,raiz->nombre);
	string_append(&path,"/Blocks/");
	string_append(&path,nombre);
	string_append(&path,".AFIP");
	return path;
}

void inicializar_bloques()
{
	char* blocks = string_new();
	string_append(&blocks,raiz->nombre);
	string_append(&blocks,"/Blocks");

	if(!existe_directorio(blocks)) mkdir(blocks, 0700);
	free(blocks);
	agregarNodo(raiz,crearNodo("/Blocks"));

	for (int i = 0; i < metadata->blocks; i++) {
		char* numero_bloque = string_itoa(i);

		char* bloque = path_bloque(numero_bloque);

		if(!existe_archivo(bloque)) {
			FILE* f = fopen(bloque,"wb+");
			fclose(f);
		}
		free(numero_bloque);
		free(bloque);
	}
}

void guardar_data_en_bloques(char* data, uint32_t bloque_inicial)

{
	int bloque_extra = 0;
	if (strlen(data)%(metadata->block_size-4) != 0)
		bloque_extra = 1;

	int bloques_necesarios = strlen(data)/(metadata->block_size-4)+bloque_extra;

	char* path_bloque;
	uint32_t numero_bloque_sig = bloque_inicial;

	for(int i=0; i<bloques_necesarios;i++)
		{
		//path_bloque = obtener_path_bloque(numero_bloque_sig);

		int arch_bloque = open(path_bloque, O_RDWR, S_IRUSR | S_IWUSR);
		char* archivo_en_memoria = mmap(NULL, metadata->block_size-4, PROT_READ | PROT_WRITE, MAP_SHARED, arch_bloque, 0);
		memcpy(archivo_en_memoria, data, metadata->block_size-4);
		if (bloques_necesarios > i)
			{//numero_bloque_sig = reservar_bloque();
			if (numero_bloque_sig >= 0){
				uint32_t* puntero_en_memoria = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, arch_bloque, metadata->block_size-4);
				*puntero_en_memoria = numero_bloque_sig;
				memcpy(puntero_en_memoria, &numero_bloque_sig, 4); // a ver si alguna de estas dos formas funciona
				munmap(puntero_en_memoria, 4);}
			}	else {log_info(logger, "No hay mas bloques disponibles");}


		munmap(archivo_en_memoria, metadata->block_size-4);
		close(arch_bloque);
		}

}
