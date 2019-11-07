#pragma once
#include "HTTP.h"

/** A multi-threaded HTTP server. */
typedef struct Server *Server;

/** Initialize an HTTP server instance. */
Server Server_new();

/** Enable/disable server hooking into signals to stop automatically (default = on). */
void Server_setHooks(Server server, int enableHooks);

/** Set server logging verbosity level. */
void Server_setVerbosity(Server server, int verbosity);

/** Add a server route. */
void Server_route(Server server, char *path, void (handler)(Request, Response));

/** Start listening for connections on the server. When finished, Server will automatically free. */
void Server_listen(Server server, int port);

/** Try to stop listening. */
void Server_stop(Server server);

