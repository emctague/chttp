#include <chttp/StringMap.h>
#include <stdlib.h>
#include <string.h>
#include <sys/rbtree.h>

typedef struct StringMapNode {
  rb_node_t node;
  char *name;
  void *value;
} *StringMapNode;

StringMapNode StringMapNode_new(const char *name, void *value) {
  StringMapNode n = malloc(sizeof(struct StringMapNode));
  n->name = strdup(name);
  n->value = value;
  return n;
}

struct StringMap {
  rb_tree_t tree;
  rb_tree_ops_t ops;
  void (*nodeCleaner)(void *node);
};

int StringMap__compare(void *context, const void *nodeA, const void *nodeB) {
  return strcmp(((const StringMapNode)nodeA)->name,
                ((const StringMapNode)nodeB)->name);
}

int StringMap__compareKey(void *context, const void *node, const void *key) {
  return strcmp(((const StringMapNode)node)->name, (const char*)key);
}

StringMap StringMap_new(void (*nodeCleaner)(void *node)) {
  StringMap map = malloc(sizeof(struct StringMap));
  map->nodeCleaner = nodeCleaner;
  map->ops.rbto_compare_nodes = StringMap__compare;
  map->ops.rbto_compare_key = StringMap__compareKey;
  map->ops.rbto_node_offset = 0;
  map->ops.rbto_context = NULL;
  rb_tree_init(&map->tree, &map->ops);
  return map;
};

void StringMap_set(StringMap map, char *name, void *value) {
  StringMapNode found = (StringMapNode)rb_tree_find_node(&map->tree, name);

  if (found) {
    map->nodeCleaner(found->value);
    found->value = value;
  } else {
    rb_tree_insert_node(&map->tree, StringMapNode_new(name, value));
  }
}

void *StringMap_get(StringMap map, char *name) {
  StringMapNode found = (StringMapNode)rb_tree_find_node(&map->tree, name);
  if (found) return found->value;
  else return NULL;
}

void StringMap_free(StringMap map) {
  StringMapNode node, tmp;
  RB_TREE_FOREACH_SAFE(node, &map->tree, tmp) {
    map->nodeCleaner(node->value);
    free(node->name);
    rb_tree_remove_node(&map->tree, node);
    free(node);
  }
  free(map);
}

StringMapNode StringMap_iterate(StringMap map, StringMapNode node) {
  return (StringMapNode)rb_tree_iterate(&map->tree, (void*)node, RB_DIR_LEFT);
}

void *StringMapNode_value(StringMapNode node) {
  return node->value;
}

char *StringMapNode_key(StringMapNode node) {
  return node->name;
}
