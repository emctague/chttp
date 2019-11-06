/**
	chttp
	
	Experimental C HTTP server.

	Copyright (C) 2019 Ethan McTague
*/

#include <string.h>
#include <stdlib.h>
#include "chttp.h"
#include "error.h"

CH_ROUTE_ERROR({
  res->status = 404;
  printf_response_body(res, "Error 404, not found!");
});

CH_ROUTE("/", {
  printf_response_body(res, "Hello, world!");
});

int main(int argc, char **argv) {
  int port = 8080;
  int verbose = 0;

  /* Parse arguments. */
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--verbose") || !strcmp(argv[i], "-v")) verbose++;
    else if (!strcmp(argv[i], "--port") || !strcmp(argv[i], "-p")) port = atoi(argv[++i]);
    else do_error(RS_BAD_USAGE, argv[0]);
  }

  ch_serve(verbose, port);
}

