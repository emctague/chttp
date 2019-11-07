#include <chttp/Server.h>
#include <chttp/Socket.h>
#include <chttp/StringMap.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <chttp/Logger.h>

struct Server {
  StringMap routes;
  Verbosity verbosity;
  int enableHooks;
  int doStop;
};

/* Used when handling interrupts. */
Server Server_globalServerPointer = NULL;

void Server_globalInterruptHandler(int _) {
  Server_globalServerPointer->doStop = 1;
}

void freeDummy(void *value) {}

Server Server_new() {
  Server server = malloc(sizeof(struct Server));
  server->routes = StringMap_new(freeDummy);
  server->enableHooks = 1;
  server->verbosity = 0;
  return server;
}

void Server_setHooks(Server server, int enableHooks) {
  server->enableHooks = enableHooks;
}

void Server_setVerbosity(Server server, int verbosity) {
  server->verbosity = verbosity;
}

void Server_route(Server server, char *path, void (handler)(Request, Response)) {
  L_log(server->verbosity, 2, "Route mapped: %s\n", path);
  StringMap_set(server->routes, path, (void*)handler);
}

void Server_stop(Server server) {
  L_log(server->verbosity, 3, "Server Stop Requested\n");
  server->doStop = 1;
}

void Server_connHandler(FILE *io, char *client_address, int uniqueID, Server server) {
  L_log(server->verbosity, 2, "Connection { ConnID = %d, IP = %s }\n", uniqueID, client_address);

  Request request = Request_new(io);
  L_log(server->verbosity, 1, "Request { ConnID = %d, IP = %s, Method = '%d', Path = '%s' }\n", uniqueID, client_address, request->method, request->path);

  if (L_will_log(server->verbosity, 3)) {
    for (int i = 0; i < StringMap_size(request->headers); i++) {
      printf("Header { ConnID = %d, Name = '%s', Value = '%s' }\n", uniqueID, StringMap_getNameAt(request->headers, i), (char*)StringMap_getValueAt(request->headers, i));
    }
  }

  void (*handler)(Request, Response) = StringMap_get(server->routes, request->path);

  Response response = Response_new();

  if (handler) {
    L_log(server->verbosity, 3, "Routed { ConnID = %d, Path = '%s' }\n", uniqueID, request->path);
    handler(request, response);
  } else {
    L_log(server->verbosity, 3, "Failed to Route { ConnID = %d, Path = '%s' }\n", uniqueID, request->path);
    response->status = 404;
    Response_printf(response, "404 Not Found");
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

void Server_listen(Server server, int port) {
  Socket socket = Socket_new(port);

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
  StringMap_free(server->routes);
}