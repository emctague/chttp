#pragma once
#include <stdio.h>
#include "http.h"

/** Represents an HTTP response. */
struct response
{
  /** The response status code - default is 200, OK. */
  int status;

  /** The response data. */
  char *data;

  /** The size of the response data. */
  size_t data_size;

  /** HTTR response headers. */
  struct header *headers;
};

/** Produces a response with the default response data. */
struct response make_response();

/** Append to the response body of the response. */
void append_response_body(struct response *response, char *data, size_t size);

/** Printf to the response body. */
void printf_response_body(struct response *response, const char *data, ...);

/** Send a response and then clean it up. */
void send_and_free_response(struct response response, FILE *f);

