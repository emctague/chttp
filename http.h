#pragma once

/** HTTP Request Methods */
enum method {
	METHOD_GET,
	METHOD_UNKNOWN
};

/** HTTP Request or Response Header */
struct header {
	/** The name of this header. */
	char *name; 

	/** The value of this header. */
	char *value;

	/** The next header in the list, or NULL. */
	struct header *next;
};

/** Add or set a header in a linked-list of headers.
 * Parameters:
 * root - The root node of the linked-list, or NULL.
 * name - The name of the header.
 * value - The value of the header.
 */
void add_header(struct header **root, char *name, char *value);

/** Get a header's value, or NULL. */
char *get_header(struct header *root, char *name);

/** Free a linked-list of headers.
 * Parameters:
 * root - The root node of a linked-list.
 */
void free_headers(struct header *root);


