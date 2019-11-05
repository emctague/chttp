/**
	chttp
	
	Experimental C HTTP server.

	Copyright (C) 2019 Ethan McTague
*/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "error.h"
#include "http.h"
#include "request.h"
#include "socket.h"

/** Indicates if the server should terminate. */
volatile int do_exit = 0;

/** Options. */
int port = 8080;
int verbose = 0;

/* Interrupt handler. */
void interrupt_handler(int _) {
	do_exit = 1;	
}

/* Connection handler. */
void conn_handler(FILE *io, char *client_address) {
	if (verbose > 2)
		printf("CONN ESTABLISHED { CLIENT = %s }\n", client_address);

	struct request req = parse_request(io);

	if (verbose > 0) printf("REQUEST { METHOD = %d, URL = '%s' }\n", req.method, req.url);
	if (verbose > 1) {
		for (struct header *header = req.headers; header; header = header->next) {
			printf("    '%s' = '%s'\n", header->name, header->value);
		}
	}

	fprintf(io, "HTTP/1.1 OK\r\nConnection: close\r\n\r\nHello, atlas!\r\n");
	free_request(req);
}

/* Socket error handler. */
int err_handler(int errno) {
	if (do_exit || errno == EINTR) {
		if (verbose > 0) puts("Terminating. Stopping listening.");
		return 0;
	} else {
		fprintf(stderr, "Socket Accept Error: %s\n", strerror(errno));
		return 1;
	}
}

int main(int argc, char **argv) {
	/* Parse arguments. */
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--verbose") || !strcmp(argv[i], "-v")) verbose++;
		else if (!strcmp(argv[i], "--port") || !strcmp(argv[i], "-p")) port = atoi(argv[++i]);
		else do_error(RS_BAD_USAGE, argv[0]);
	}


	/* Set interrupt handler. */
	struct sigaction action;
	action.sa_handler = interrupt_handler;
	action.sa_sigaction = NULL;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGINT, &action, NULL);

	struct smart_socket sock = new_sock(port);

	printf("Listening on *:%d\n", port);

	while (!do_exit && accept_conn(&sock, conn_handler, err_handler));

	free_sock(sock);
}
