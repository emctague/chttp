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


typedef struct {
  int exists, pos;
} BinaryResult;

BinaryResult StringMap_binarySearch(char **list, int size, char *name) {
  int left = 0;
  int right = size - 1;
  int mid = 0;
  int ord = 0;

  if (size == 0) return (BinaryResult){0, 0};
  else if (size == 1) {
    ord = strcmp(list[0], name);
    return (BinaryResult){!ord, ord > 0 ? mid : mid + 1};
  }

  while (left <= right) {
    mid = (left + right) / 2;
    ord = strcmp(list[mid], name);
    if (ord == 0) return (BinaryResult){1, mid};
    else if (ord > 0) right = mid - 1;
    else left = mid + 1;
  }

  return (BinaryResult){0, ord > 0 ? mid : mid + 1};
}


void StringMap_set(StringMap map, char *name, void *value) {
  BinaryResult putAt = StringMap_binarySearch(map->names, map->size, name);

  if (putAt.exists) {
    map->nodeCleaner(map->values[putAt.pos]);
    map->values[putAt.pos] = value;
  } else {
    map->names = realloc(map->names, (map->size + 1) * sizeof(char*));
    map->values = realloc(map->values, (map->size + 1) * sizeof(void*));

    for (int i = map->size; i > putAt.pos; i--) {
      map->names[i] = map->names[i - 1];
      map->values[i] = map->values[i - 1];
      map->names[i - 1] = 0;
    }

    map->names[putAt.pos] = strdup(name);
    map->values[putAt.pos] = value;
    map->size++;
  }
}


void *StringMap_get(StringMap map, char *name) {
  BinaryResult found = StringMap_binarySearch(map->names, map->size, name);

  if (found.exists) return map->values[found.pos];
  else return NULL;
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
