#pragma once

/** A sorted map that maps strings to an arbitrary type. */
typedef struct StringMap *StringMap;

/** A node in a StringMap, used here only during iteration. */
typedef struct StringMapNode *StringMapNode;

/** Initialize a StringMap. */
StringMap StringMap_new(void (*nodeCleaner)(void *node));

/** Set a value in a StringMap. */
void StringMap_set(StringMap map, char *name, void *value);

/** Get a value in a StringMap. */
void *StringMap_get(StringMap map, char *name);

/** Free up a StringMap. */
void StringMap_free(StringMap map);

/** Iterate over a StringMap. Start with 'node' as NULL */
StringMapNode StringMap_iterate(StringMap map, StringMapNode node);

/** Get the value of a StringMapNode. */
void *StringMapNode_value(StringMapNode node);

/** Get the key of a StringMapNode. */
char *StringMapNode_key(StringMapNode node);
