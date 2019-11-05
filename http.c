#include "http.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void add_header(struct header **root, char *name, char *value) {
	
	struct header *last = *root;
	for (struct header *header = *root; header; header = header->next) {
		last = header;
		if (!strcmp(header->name, name)) {
			free(header->value);
			header->value = strdup(value);
			return;
		}
	}

	struct header *header = malloc(sizeof(struct header));
	header->name = strdup(name);
	header->value = strdup(value);
	header->next = NULL;

	if (!*root) *root = header;
	else last->next = header;
}


char *get_header(struct header *root, char *name) {
	for (struct header *header = root; header; header = header->next) {
		if (!strcmp(header->name, name)) return header->value;
	}

	return NULL;
}


void free_headers(struct header *root)
{
	struct header *header = root;

	while (header) {
		struct header *temp = header->next;
		free(header->name);
		free(header->value);
		free(header);
		header = temp;
	}
}

