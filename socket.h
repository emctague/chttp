#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>

/** Represents a C socket. */
struct smart_socket {
	int sock;
	struct sockaddr_in addr;
	socklen_t len;
};

/** Establish a socket listening on the given port.
 * Parameters:
 * port - The TCP port to listen on
 * Returns:
 * A socket that should be closed with free_sock.
 */
struct smart_socket new_sock(int port);

/** Clean up a socket.
 * Parameters:
 * socket - The socket to clean up.
 */
void free_sock(struct smart_socket socket);

/** A connection handler. */
typedef void(*pfn_conn_handler)(FILE *io, char *client_address);

/** A connection error handler. */
typedef int(*pfn_err_handler)(int errno);

/** Accept new socket connections.
 * Parameters:
 * socket - The socket to accept a connection on.
 * Returns:
 * 1 to continue, or err_handler result if errors occurred.
 */
int accept_conn(struct smart_socket *socket, pfn_conn_handler handler, pfn_err_handler err_handler);

