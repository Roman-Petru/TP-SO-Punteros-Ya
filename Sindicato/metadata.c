#include "metadata.h"

t_nodo_arbol* raiz;
t_metadata* metadata;
t_bitarray* mapa_bits;
pthread_mutex_t mutex_mapa_bit;

void inicializar_metadata()
{

	char* puntoMontaje = config_get_string_value(config, "PUNTO_MONTAJE");
	raiz = crearNodo(puntoMontaje);
	if (!existe_directorio(puntoMontaje))
		{if (0 != mkdir(puntoMontaje, 0700)){
			perror("mkdir");		}
			printf("que onda");}

	char* meta = string_new();
	string_append(&meta,raiz->nombre);
	string_append(&meta,"/Metadata");

	char* bitm = string_new();
	string_append(&bitm,meta);
	string_append(&bitm,"/Bitmap.bin");
//=============================ARCHIVO METADATA===========================//

	if(!existe_directorio(meta)) 	mkdir(meta, 0700);
	agregarNodo(raiz,crearNodo("/Metadata"));
	string_append(&meta,"/Metadata.AFIP");

	if(!existe_archivo(meta)) {
		FILE* metadata = fopen(meta,"wb+");
		fprintf(metadata,"BLOCK_SIZE=64\n");
		fprintf(metadata,"BLOCKS=8\n");
		fprintf(metadata,"MAGIC_NUMBER=AFIP");
		fclose(metadata);
	}
	int fd = open(meta, O_RDONLY, S_IRUSR | S_IWUSR);
	char* metadata_char = mmap(NULL, 200, PROT_READ, MAP_PRIVATE, fd, 0);

	char** aux = string_n_split(metadata_char, 3, "\n");

	metadata = malloc(sizeof(t_metadata));
	metadata->block_size = strtol(aux[0]+11, NULL, 10);
	metadata->blocks = strtol(aux[1]+7, NULL, 10);
	metadata->magic_number = aux[2]+13;

	if (strcmp(metadata->magic_number, "AFIP") != 0)
		{log_info(logger, "Se detecto un MAGIC NUMBER que no es AFIP, cerrando sindicato");
		exit(0);}

	free(aux[0]); free(aux[1]); free(aux);
	munmap(metadata_char, 200);
	close(fd);

//===============================BITMAP=================================//

	pthread_mutex_init(&mutex_mapa_bit, NULL);
	if (!existe_archivo(bitm)) crear_bitmap(bitm);
	else {

		int fd = open(obtener_path_bitmap(), O_RDONLY, S_IRUSR | S_IWUSR);
			char* bit_array_aux = mmap(NULL, metadata->blocks/8, PROT_READ, MAP_PRIVATE, fd, 0);

			char* para_mapa = calloc(1,metadata->blocks/8);

			memcpy(para_mapa, bit_array_aux, metadata->blocks/8);
			munmap(bit_array_aux, metadata->blocks/8);
			close(fd);

			mapa_bits = bitarray_create_with_mode(para_mapa, metadata->blocks/8, LSB_FIRST);


		}

}


void crear_bitmap(char* bitm)
{

	FILE* fBitmap = fopen(bitm,"wb+");

	if(fBitmap==NULL)
		{log_info(logger, "No se pudo crear bit map"); exit(-1);}

	mapa_bits = bitarray_create_with_mode(string_repeat('\0',metadata->blocks), metadata->blocks/8, LSB_FIRST);

	fwrite(mapa_bits->bitarray,mapa_bits->size,1,fBitmap);
	fclose(fBitmap);
}


bool existe_archivo(char *path)
{
  int fd = open(path, O_RDONLY);
  if (fd < 0) return false;
  close(fd);
  return true;
}


bool existe_directorio(char* path) {
	struct stat buf;
	return stat(path, &buf) != -1;
}
