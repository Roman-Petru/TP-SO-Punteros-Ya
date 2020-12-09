#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "config_string.h"
#include "commons/string.h"


t_config_string *config_string_create(char* buffer) {

	t_config_string *config = malloc(sizeof(t_config_string));


	config->properties = dictionary_create();

	char** lines = string_split(buffer, "\n");

	void add_cofiguration(char *line) {
		if (!string_starts_with(line, "#")) {
			char** keyAndValue = string_n_split(line, 2, "=");
			dictionary_put(config->properties, keyAndValue[0], keyAndValue[1]);
			free(keyAndValue[0]);
			free(keyAndValue);
		}
	}
	string_iterate_lines(lines, add_cofiguration);
	string_iterate_lines(lines, (void*) free);

	free(lines);
	//free(buffer);


	return config;
}

bool config_string_has_property(t_config_string *self, char* key) {
	return dictionary_has_key(self->properties, key);
}

char *config_string_get_string_value(t_config_string *self, char *key) {
	return dictionary_get(self->properties, key);
}

int config_string_get_int_value(t_config_string *self, char *key) {
	char *value = config_string_get_string_value(self, key);
	return atoi(value);
}

long config_string_long_value(t_config_string *self, char *key) {
	char *value = config_string_get_string_value(self, key);
	return atol(value);
}

double config_string_get_double_value(t_config_string *self, char *key) {
	char *value = config_string_get_string_value(self, key);
	return atof(value);
}

char** config_string_get_array_value(t_config_string* self, char* key) {
	char* value_in_dictionary = config_string_get_string_value(self, key);
	return string_get_string_as_array(value_in_dictionary);
}

int config_string_keys_amount(t_config_string *self) {
	return dictionary_size(self->properties);
}

void config_string_destroy(t_config_string *config) {
	dictionary_destroy_and_destroy_elements(config->properties, free);
	free(config);
}

void config_string_set_value(t_config_string *self, char *key, char *value) {
	config_string_remove_key(self, key);

	char* duplicate_value = string_duplicate(value);

	dictionary_put(self->properties, key, (void*)duplicate_value);
}

void config_string_remove_key(t_config_string *self, char *key) {
	t_dictionary* dictionary = self->properties;

	if(dictionary_has_key(dictionary, key)) {
		dictionary_remove_and_destroy(dictionary, key, free);
	}
}
