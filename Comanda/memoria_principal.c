#include "memoria_principal.h"
#include "comanda.h"

t_list* tabla_marcos;
int cantidad_frames;
t_bitarray* mapa_bits;

void crear_marco()
{
t_marco* marco = malloc(sizeof(t_marco));
}


void inicializar_memoria_principal()
{
	cantidad_frames = (config_get_int_value(config, "TAMANIO_MEMORIA") / 32 );
	tabla_marcos = list_create();

	void* mentira = malloc(cantidad_frames / 8);
	mapa_bits = bitarray_create_with_mode(mentira, cantidad_frames / 8, MSB_FIRST);

	for (int i=0; i < bitarray_get_max_bit(mapa_bits); i++)
	{
		bitarray_set_bit(mapa_bits, i);
		list_add(tabla_marcos, crear_marco());
	}
}


