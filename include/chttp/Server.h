#pragma once
#include "HTTP.h"
#include "Error.h"

/** A multi-threaded HTTP server. */
typedef struct Server *Server;

/** An HTTP request handler. Return type is Result<void>. */
typedef Result(*PFNRouteHandler)(Request, Response);

/** Initialize an HTTP server instance. */
Server Server_new();

/** Enable/disable server hooking into signals to stop automatically (default = on). */
void Server_setHooks(Server server, int enableHooks);

/** Set server logging verbosity level. */
void Server_setVerbosity(Server server, int verbosity);

/** Set server 404 error handler. */
void Server_route404(Server server, PFNRouteHandler handler);

/** Set server 500 error handler. */
void Server_route500(Server server, PFNRouteHandler handler);

/** Add a server route. */
void Server_route(Server server, char *path, PFNRouteHandler handler);

/** Start listening for connections on the server. When finished, Server will automatically free.
 * Returns Result<void>*/
Result Server_listen(Server server, int port);

/** Try to stop listening. */
void Server_stop(Server server);

