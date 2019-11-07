#pragma once
#include <stdio.h>

/** TCP/IP Socket Object. */
typedef struct Socket *Socket;

/** Function pointer type to handle socket connections. */
typedef void (*PFNSocketConnHandler)(FILE *io, char *client_address, int uniqueID, void *userPointer);

/** Function pointer type to handle socket connection errors.
 * Return value is passed onto Socket_accept. */
typedef int(*PFNSocketErrHandler)(int errno, int uniqueID, void *userPointer);

/** Establish a socket listening on the given port. */
Socket Socket_new (int port);

/** Free a socket. */
void Socket_free (Socket socket);

/** Accept a socket connection. Returns 1 to continue, or
 * an error value if an error occurred. */
int Socket_accept(Socket socket, PFNSocketConnHandler connHandler, PFNSocketErrHandler errHandler, void *userPointer);

