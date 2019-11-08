#include <chttp/Server.h>
#include <chttp/Socket.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <chttp/Logger.h>
#include "thirdparty/uthash.h"

/* Default error 500 handler. */
Result Server_default500Handler(Request req, Response res) {
  Response_printf(res, "Error 500 - Internal Server Error");
  return ResultOK(NULL);
}

/* Default error 404 handler. */
Result Server_default404Handler(Request req, Response res) {
  Response_printf(res, "Error 404 - Not Found");
  return ResultOK(NULL);
}

typedef struct Route {
  char *key;
  PFNRouteHandler value;
  UT_hash_handle hh;
} *Route;

struct Server {
  Route routes;
  Verbosity verbosity;
  int enableHooks;
  int doStop;
  PFNRouteHandler handle500;
  PFNRouteHandler handle404;
};

/* Used when handling interrupts. */
Server Server_globalServerPointer = NULL;

void Server_globalInterruptHandler(int _) {
  Server_globalServerPointer->doStop = 1;
}

void freeDummy(void *value) {}

Server Server_new() {
  Server server = malloc(sizeof(struct Server));
  server->doStop = 0;
  server->routes = NULL;
  server->enableHooks = 1;
  server->verbosity = 0;
  server->handle500 = Server_default500Handler;
  server->handle404 = Server_default404Handler;

  return server;
}

void Server_setHooks(Server server, int enableHooks) {
  server->enableHooks = enableHooks;
}

void Server_setVerbosity(Server server, int verbosity) {
  server->verbosity = verbosity;
}

void Server_route404(Server server, PFNRouteHandler handler) {
  L_log(server->verbosity, 2, "404 Handler Mapped\n");
  server->handle404 = handler;
}

void Server_route500(Server server, PFNRouteHandler handler) {
  L_log(server->verbosity, 2, "500 Handler Mapped\n");
  server->handle500 = handler;
}

void Server_route(Server server, char *path, PFNRouteHandler handler) {
  L_log(server->verbosity, 2, "Route mapped: %s\n", path);

  Route r = malloc(sizeof(struct Route));
  r->key = strdup(path);
  r->value = handler;
  HASH_ADD_KEYPTR(hh, server->routes, r->key, strlen(r->key), r);
}

void Server_stop(Server server) {
  L_log(server->verbosity, 3, "Server Stop Requested\n");
  server->doStop = 1;
}

void Server_doError500(Server server, Request request, Response response, int uniqueID) {
  response->status = 500;
  OkOr(void*, (server->handle500(request, response)), {
      L_log(server->verbosity, 1, "Route Error { ConnID = %d, Path = '%s', Handler = 500, Message = '%s' }\n", uniqueID, request->path, OkOrMessage);
      Response_printf(response, "Error 500");
  });
}

void Server_connHandler(FILE *io, char *client_address, int uniqueID, Server server) {
  L_log(server->verbosity, 2, "Connection { ConnID = %d, IP = %s }\n", uniqueID, client_address);

  Request request = Request_new(io);
  L_log(server->verbosity, 1, "Request { ConnID = %d, IP = %s, Method = '%d', Path = '%s' }\n", uniqueID, client_address, request->method, request->path);

  /*if (L_will_log(server->verbosity, 3)) {
    StringMapNode node = NULL;
    while ((node = StringMap_iterate(request->headers, node))) {
      printf("Header { ConnID = %d, Name = '%s', Value = '%s' }\n", uniqueID, StringMapNode_key(node), (char*)StringMapNode_value(node));
    }
  } TODO REIMPLEMENT WITH UTHASH */

  Route found = NULL;
  HASH_FIND_STR(server->routes, request->path, found);

  PFNRouteHandler handler = found ? found->value : NULL;

  Response response = Response_new();

  if (handler) {
    L_log(server->verbosity, 3, "Routed { ConnID = %d, Path = '%s' }\n", uniqueID, request->path);
    OkOr(void*, (handler(request, response)), {
        L_log(server->verbosity, 1, "Route Error { ConnID = %d, Path = '%s', Message = '%s' }\n", uniqueID, request->path, OkOrMessage);
        Server_doError500(server, request, response, uniqueID);
    });
  } else {
    L_log(server->verbosity, 3, "Failed to Route { ConnID = %d, Path = '%s' }\n", uniqueID, request->path);
    response->status = 404;
    OkOr(void*, (server->handle404(request, response)), {
        L_log(server->verbosity, 1, "Route Error { ConnID = %d, Path = '%s', Handler = 404, Message = '%s' }\n", uniqueID, request->path, OkOrMessage);
        Server_doError500(server, request, response, uniqueID);
    });
  }

  Response_send(response, io);
  Request_free(request);

  L_log(server->verbosity, 2, "Disconnect { ConnID = %d }\n", uniqueID);
}

int Server_errHandler(int errno, int uniqueID, Server server) {
  if (server->doStop || errno == EINTR) {
    L_log(server->verbosity, 2, "Server Interrupt { ConnID = %d, errno = %d }\n", uniqueID, errno);
    return 0;
  } else {
    fprintf(stderr, "Socket Accept Error { ConnID = %d, errno = %d, strerror = '%s' }\n", uniqueID, errno, strerror(errno));
    return 1;
  }
}

Result Server_listen(Server server, int port) {
  Socket socket = OkOr(Socket, (Socket_new(port)), { return OkOrResult; });

  if (server->enableHooks) {
    struct sigaction action;
    action.sa_handler = Server_globalInterruptHandler;
    action.sa_sigaction = NULL;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    
    Server_globalServerPointer = server;
  }

  L_log(server->verbosity, 0, "Listening { Port = %d }\n", port);

  while (!server->doStop && Socket_accept(socket, (PFNSocketConnHandler)Server_connHandler, (PFNSocketErrHandler)Server_errHandler, server));

  L_log(server->verbosity, 1, "Stopped Listening");

  Socket_free(socket);

  Route r, tmp;
  HASH_ITER(hh, server->routes, r, tmp) {
    HASH_DEL(server->routes, r);
    free(r->key);
    free(r);
  }

  return ResultOK(NULL);
}
