#include "router.h"
#include <stdlib.h>
#include <string.h>

struct router router_new(pfn_handler notfound_handler) {
  struct router router;
  router.routes = NULL;
  router.notfound_handler = notfound_handler;
  return router;
};


void add_route(struct router *router, char *path, pfn_handler handler) {
  struct route *route = malloc(sizeof(struct route));  
  route->path = strdup(path);
  route->handler = handler;
  route->next = router->routes;
  router->routes = route;
}


void router_free(struct router *router) {
  struct route *route = router->routes;
  while (route) {
    struct route *next = route->next;
    free(route->path);
    free(route);
    route = next;
  }
}

struct response router_route(struct router *router, struct request request) {
  struct response res = make_response();

  for(struct route *route = router->routes; route; route = route->next) {
    if (!strcmp(route->path, request.url)) {
      route->handler(&request, &res);
      free_request(request);
      return res;
    }
  }

  router->notfound_handler(&request, &res);
  free_request(request);
  return res;
}
