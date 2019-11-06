#pragma once
#include "http.h"
#include "request.h"
#include "response.h"

/** A pfn_handler is a route handler function that produces a response. */
typedef void (*pfn_handler)(struct request *request, struct response *response);

/** A specific route matches a certain path and calls a certain function. */
struct route {
  /** The path this route works on. */
  char *path;

  /** The handler that produces a response on this route. */
  pfn_handler handler;

  /** The next route in the list. */
  struct route *next;
};

/** A router stores a set of routes, which handle certain paths. */
struct router {
  /** The list of routes to try. */
  struct route *routes;

  /** The handler when all other handlers fail. */
  pfn_handler notfound_handler;
};

/** Create a new router. */
struct router router_new(pfn_handler notfound_handler);

/** Add a new route to the router. */
void add_route(struct router *router, char *path, pfn_handler handler);

/** Clean up a router. */
void router_free(struct router *router);

/** Get a response for the given request. (Frees the request in the process.) */
struct response router_route(struct router *router, struct request request);

