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

//=================================="SOLICITAR ESPACIO EN DISCO"==================================//

int reservar_bloque()
{

	int index;
	bool encontro = true;

	pthread_mutex_lock(&mutex_mapa_bit);
	for(index=0;encontro && index<mapa_bits->size;index++)
	{
		encontro = bitarray_test_bit(mapa_bits, index);
		if (!encontro)
			bitarray_set_bit(mapa_bits, index);
	}

	/*int test;
	for (int i=0; i < 124; i++)
		{test = bitarray_test_bit(mapa_bits, i);
		printf("%d", test);}*/

	if (!encontro)
		{
		int fd = open(obtener_path_bitmap(), O_RDWR, S_IRUSR | S_IWUSR);
		void* bit_array_aux = mmap(NULL, mapa_bits->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(bit_array_aux, mapa_bits->bitarray, mapa_bits->size);
		munmap(bit_array_aux, mapa_bits->size);
		close(fd);

		/*FILE* fBitmap = fopen(obtener_path_bitmap(),"wb+");
			fwrite(mapa_bits->bitarray,mapa_bits->size,1,fBitmap);
			fclose(fBitmap);*/
		}



	pthread_mutex_unlock(&mutex_mapa_bit);
	return !encontro ? index-1 : -1;

}

//=================================="ESCRIBIR EN DISCO"==================================//

void guardar_data_en_bloques(char* data, uint32_t bloque_inicial, t_list* bloques_siguientes)

{
	int bloque_extra = 0;
	if (strlen(data)%(metadata->block_size-4) != 0)
		bloque_extra = 1;

	int bloques_necesarios = strlen(data)/(metadata->block_size-4)+bloque_extra;
	int numero_bloque_sig = bloque_inicial;

	for(int i=0; i<bloques_necesarios;i++)
		{
		char* path_bloque = obtener_path_bloque(numero_bloque_sig);

		int arch_bloque = open(path_bloque, O_RDWR, S_IRWXU | S_IRWXG);
		posix_fallocate(arch_bloque, 0, metadata->block_size);
		void* archivo_en_memoria = mmap(NULL, metadata->block_size, PROT_READ | PROT_WRITE, MAP_SHARED, arch_bloque, 0);
		memcpy(archivo_en_memoria, data, metadata->block_size-4);
		log_info(logger, "Se escribio en el bloque %d", numero_bloque_sig);

		if (bloques_necesarios > i+1)
			{data = data + metadata->block_size-4;
//======================BUSCO BLOQUE SIGUIENTE O RESERVO NUEVO SI NO HAY==========================//
			if (!list_is_empty(bloques_siguientes))
				numero_bloque_sig = (uint32_t) list_remove(bloques_siguientes, 0);
			else numero_bloque_sig = reservar_bloque();

//======================GUARDO PUNTERO AL SIG BLOQUE EN EL BLOQUE==========================//
			if (numero_bloque_sig >= 0){
				uint32_t* puntero_a_numero = archivo_en_memoria + metadata->block_size - 4;
				//*puntero_a_numero = numero_bloque_sig;
				memcpy(puntero_a_numero, &numero_bloque_sig, 4);
				munmap(puntero_a_numero, 4);} else {log_info(logger, "No hay mas bloques disponibles");
			}	}


		munmap(archivo_en_memoria, metadata->block_size);
		free(path_bloque);
		close(arch_bloque);
		}

	//free(data);
}
