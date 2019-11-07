#pragma once

/** A sorted map that maps strings to an arbitrary type. */
typedef struct StringMap *StringMap;

/** Initialize a StringMap. */
StringMap StringMap_new(void (*nodeCleaner)(void *node));

/** Set a value in a StringMap. */
void StringMap_set(StringMap map, char *name, void *value);

/** Get a value in a StringMap. */
void *StringMap_get(StringMap map, char *name);

/** Free up a StringMap. */
void StringMap_free(StringMap map);

/** Get the size of the StringMap. */
int StringMap_size(StringMap map);

/** Get an item by index from the StringMap. */
void *StringMap_getValueAt(StringMap map, int index);

/** Get an item's name by index. */
char *StringMap_getNameAt(StringMap map, int index);

