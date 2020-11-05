#include archivos.h
#include <dirent.h> //revisar


///CREAR DIRECTORIO FILES
void crearDirectorioFiles() {

	log_info(logger,"Creando directorio Files");

	char* aux = string_new();
	string_append(&aux,pathPtoMnt());
	string_append(&aux,"/Files");
	agregarNodo(raiz,crearNodo("/Files"));

	if (!existeDirectorio(aux)) {
		mkdir(aux, 0700);
		string_append(&aux,"/metadata.bin");
		FILE* metadata = fopen(aux,"wb+");
		fputs("DIRECTORY=Y\n",metadata);
		fclose(metadata);
		free(aux);
	} else {
		log_info(logger,"Levantando directorio files");
		DIR* dir = opendir(aux);
		struct dirent* entry;   //revisar TODO

		if (dir == NULL) {
			log_error(logger,"No se pudo abrir correctamente el directorio files");
		} else {
			while((entry = readdir(dir))) {
				if (!sonIguales(entry->d_name,"metadata.bin") && !sonIguales(entry->d_name,".") && !sonIguales(entry->d_name,"..")) {
					log_info(logger,"Leyendo archivo %s",entry->d_name);
					NodoArbol* nodo = crearNodo(entry->d_name);
					agregarNodo(directorioFiles(),nodo);
					//nuevoSemaforo(nodo->nombre);
				}
			}
		}
		free(aux);
	closedir(dir);
	}
}


///CREAR DIRECTORIO METADATA

void crearDirectorioMetadata() {

	log_info(logger,"Creando directorio Metadata");

	char* aux = string_new();
	string_append(&aux,raiz->nombre);
	string_append(&aux,"/Metadata");

	if(!existeDirectorio(aux)) {
		mkdir(aux, 0700);
	}

	agregarNodo(raiz,crearNodo("/Metadata"));

	char* aux2 = string_new();
	string_append(&aux2,aux);
	string_append(&aux2,"/bitmap.bin");

	string_append(&aux,"/metadata.bin");

	if(!existeArchivo(aux)) {
		FILE* metadata = fopen(aux,"wb+");
		fprintf(metadata,"BLOCK_SIZE=%d\n",config_get_int_value(config,"BLOCK_SIZE"));
		fprintf(metadata,"BLOCKS=%d\n",config_get_int_value(config,"BLOCKS"));
		fprintf(metadata,"MAGIC_NUMBER=%s\n",config_get_string_value(config,"MAGIC_NUMBER"));
		fclose(metadata);
	}

	t_config* conf = CrearConfig(aux);

	configFS.cantidadBlocks = config_get_int_value(conf,"BLOCKS");
	configFS.tamanioBlocks = config_get_int_value(conf,"BLOCK_SIZE");
	configFS.magicNumber = config_get_string_value(conf,"MAGIC_NUMBER");

	config_destroy(conf);

	if (!existeArchivo(aux2)) crearBitmap(aux2);
	else {
		log_info(logger,"Levantando bitmap");
		FILE* bm = fopen(aux2,"rb");
		char* bitarray = calloc(1,configFS.cantidadBlocks);
		fread(bitarray,configFS.cantidadBlocks,1,bm);
		bitmap = bitarray_create_with_mode(bitarray, configFS.cantidadBlocks, LSB_FIRST);
		fclose(bm);
	}

	free(aux2);
	free(aux);
}

///CREAR DIRECTORIO BLOCKS



///CREAR BITMAP
void crearBitmap(char* path) {

	log_info(logger,"Creando bitmap");
	bitmap = bitarray_create_with_mode(string_repeat('\0',configFS.cantidadBlocks), configFS.cantidadBlocks/8, LSB_FIRST);

	FILE* fBitmap = fopen(path,"wb+");
	fwrite(bitmap->bitarray,bitmap->size,1,fBitmap);
	fclose(fBitmap);
}

///ABRIR ARCHIVO
void abrir(t_config* conf) {
	config_set_value(conf,"OPEN","Y");
	ActualizarConfig(conf);
}


///CERRAR ARCHIVO
void cerrar(t_config* conf) {
	config_set_value(conf,"OPEN","N");
	ActualizarConfig(conf);
}

///LEER ARCHIVO
