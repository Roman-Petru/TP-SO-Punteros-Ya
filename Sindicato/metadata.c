#include "metadata.h"

t_nodo_arbol* raiz;
t_metadata* metadata;
t_bitarray* mapa_bits;

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
		fprintf(metadata,"BLOCKS=5192\n");
		fprintf(metadata,"MAGIC_NUMBER=AFIP\n");
		fclose(metadata);
	}
	int fd = open(meta, O_RDONLY, S_IRUSR | S_IWUSR);
	char* metadata_char = mmap(NULL, 200, PROT_READ, MAP_PRIVATE, fd, 0);

	char** aux = string_n_split(metadata_char, 2, "\n");

	metadata = malloc(sizeof(t_metadata));
	metadata->block_size = strtol(aux[0]+11, NULL, 10);
	metadata->blocks = strtol(aux[1]+7, NULL, 10);

	free(aux[0]); free(aux[1]); free(aux);
	munmap(metadata_char, 200);
	close(fd);

//===============================BITMAP=================================//

	if (!existe_archivo(bitm)) crear_bitmap(bitm);
	else {

		FILE* bm = fopen(bitm,"rb");

		char* bitarray = calloc(1,metadata->blocks/8);
		fread(bitarray, metadata->blocks/8, 1, bm);
		mapa_bits = bitarray_create_with_mode(bitarray, metadata->blocks/8, LSB_FIRST);
		fclose(bm);
	}
}


void crear_bitmap(char* bitm)
{
	mapa_bits = bitarray_create_with_mode(string_repeat('\0',metadata->blocks), metadata->blocks/8, LSB_FIRST);
	FILE* fBitmap = fopen(bitm,"wb+");

	if(fBitmap==NULL)
		{log_info(logger, "No se pudo crear bit map"); exit(-1);}

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
