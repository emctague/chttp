#include <chttp/StringMap.h>
#include <stdlib.h>
#include <string.h>

struct StringMap {
  char **names;
  void **values;
  int size;
  void (*nodeCleaner)(void *node);
};


StringMap StringMap_new(void (*nodeCleaner)(void *node)) {
  StringMap map = malloc(sizeof(struct StringMap));
  map->names = NULL;
  map->values = NULL;
  map->size = 0;
  map->nodeCleaner = nodeCleaner;
  return map;
};

/* Represents the state of a bsearch binary search. Keeps track of last-visited key. */
typedef struct {
  char *key;
  char **lastVisited;
} StringMap_searchState;

/* Strcmp two strings and update search state - used for bsearch. */
int StringMap_strcmp(const void *a, const void *b) {
  StringMap_searchState *state = (StringMap_searchState*)a;
  state->lastVisited = (char**)b;
  return strcmp(state->key, *(const char**)b);
}


void StringMap_set(StringMap map, char *name, void *value) {
  /* Search for the name in the map, but keep track of last-visited item, too. */
  StringMap_searchState state = { name, map->names };
  char **found = bsearch(&state, map->names, map->size, sizeof(char*), StringMap_strcmp);

  if (found) {
    printf("updating: %s\n", name);
    /* When we *did* find the item, update it. */
    int index = (found - map->names) / sizeof(char*);
    map->nodeCleaner(map->values[index]);
    map->values[index] = value;
  } else {
    printf("inserting: %s\n", name);
    /* Otherwise, we need to insert. */
    int index;

    /* If the map is empty our index is zero. */
    if (map->size == 0) index = 0;
    else {
      /* If the map is NOT empty, we select an insertion point depending on the last-checked value. */
      index = (state.lastVisited - map->names) / sizeof(char*);
      if (strcmp(map->values[index], name) > 0) index++;
    }

    map->names = realloc(map->names, (map->size + 1) * sizeof(char*));
    map->values = realloc(map->values, (map->size + 1) * sizeof(void*));

    for (int i = map->size; i > index; i--) {
      map->names[i] = map->names[i - 1];
      map->values[i] = map->values[i - 1];
      map->names[i - 1] = 0;
    }

    map->names[index] = strdup(name);
    map->values[index] = value;
    map->size++;
  }
}


void *StringMap_get(StringMap map, char *name) {
  StringMap_searchState searchState = { name, map->names };
  char **found = bsearch(&searchState, map->names, map->size, sizeof(char*), StringMap_strcmp);
  printf("searching: %s\n", name);
  if (found) {
    printf("found: %s\n", name);
    return map->values[(found - map->names) / sizeof(char*)];
  } else return NULL;
}


void StringMap_free(StringMap map) {
  for (int i = 0; i < map->size; i++) {
    free(map->names[i]);
    map->nodeCleaner(map->values[i]);
  }
  free(map->names);
  free(map->values);
  free(map);
}


int StringMap_size(StringMap map) {
  return map->size;
}

void *StringMap_getValueAt(StringMap map, int index) {
  return map->values[index];
}

char *StringMap_getNameAt(StringMap map, int index) {
  return map->names[index];
}
