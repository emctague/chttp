#include "chttp.h"
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "router.h"
#include "socket.h"

/** Indicates the level of verbosity to use. */
int ch_verbosity_level = 0;

/** Indicates if serving loop should exit. */
int ch_do_exit = 0;


struct router ch_router = {
  .routes = NULL,
  .notfound_handler = NULL
};


/** Handles SIGINT interrupt. */
void ch_interrupt_handler(int _) {
  ch_do_exit = 1;
}

/** Handles socket errors. */
int ch_err_handler(int errno) {
  if (ch_do_exit || errno == EINTR) {
    ch_log(1, "Interrupt Detected, Terminating");
    return 0;
  } else {
    fprintf(stderr, "Socket Accept Error: %s\n", strerror(errno));
    return 1;
  }
}

/** Handles connections. */
void ch_conn_handler(FILE *io, char *client_address) {
  ch_log(3, "CONNECTED { CLIENT = %s }\n", client_address);

  struct request req = parse_request(io);

  ch_log(1, "REQUEST { METHOD = %d, URL = '%s' }\n", req.method, req.url);
  if (ch_verbosity_level >= 2) {
    for (struct header *header = req.headers; header; header = header->next) {
      printf("    %s: %s\n", header->name, header->value);
    }
  }

  struct response res = router_route(&ch_router, req);
  send_and_free_response(res, io);
  ch_log(3, "DISCONNECTED { CLIENT = %s }\n", client_address);
}



void ch_log(int verbosity_level, const char *fmt, ...)
{
  if (ch_verbosity_level >= verbosity_level) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  }
}


void ch_route(char *path, pfn_handler handler) {
  add_route(&ch_router, path, handler);
}


void ch_route_error(pfn_handler notfound_handler) {
  ch_router.notfound_handler = notfound_handler;
}


void ch_serve(int verbosity_level, int port)
{
  ch_verbosity_level = verbosity_level;

  struct smart_socket sock = new_sock(port);

  struct sigaction action;
  action.sa_handler = ch_interrupt_handler;
  action.sa_sigaction = NULL;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGINT, &action, NULL);

  ch_log(0, "Listening on *:%d\n", port);

  while (!ch_do_exit && accept_conn(&sock, ch_conn_handler, ch_err_handler)) {}

  router_free(&ch_router);
  free_sock(sock);
}

