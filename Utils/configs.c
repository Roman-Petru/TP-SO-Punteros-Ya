#include "configs.h"
#include <stdlib.h>

t_config* leer_config(char* config_a_leer)
{
	return config_create(config_a_leer);
}
