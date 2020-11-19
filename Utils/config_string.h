#ifndef CONFIG_S_H_
#define CONFIG_S_H_

#include "commons/collections/dictionary.h"

typedef struct {
	t_dictionary *properties;
} t_config_string;


t_config_string *config_string_create(char* buffer);
bool config_string_has_property(t_config_string *self, char* key);
char *config_string_get_string_value(t_config_string *self, char *key);
int config_string_get_int_value(t_config_string *self, char *key);
long config_string_long_value(t_config_string *self, char *key);
double config_string_get_double_value(t_config_string *self, char *key);
char** config_string_get_array_value(t_config_string* self, char* key);
int config_string_keys_amount(t_config_string *self);
void config_string_destroy(t_config_string *config);
void config_string_remove_key(t_config_string *self, char *key) ;
void config_string_set_value(t_config_string *self, char *key, char *value);

#endif
