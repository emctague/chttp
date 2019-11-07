#pragma once
#include "StringMap.h"
#include <stdio.h>
#include <string.h>


/** HTTP Request Methods */
typedef enum HTTPMethod {
  HTTPMethodGET,
  HTTPMethodPOST,
  HTTPMethodHEAD,
  HTTPMethodUnknown
} HTTPMethod;


/** HTTP Request */
typedef struct Request {
  HTTPMethod method; /**< Request method. */
  char *path; /**< Request path. */
  StringMap headers; /**< StringMap of HTTP headers. */
  void *body; /**< Request body, if included. */
  size_t body_size; /**< Request body size. */
} *Request;


/** HTTP Response */
typedef struct Response {
  int status; /**< Response status code. */
  void *data; /**< Response data. */
  size_t data_size; /**< Response data size. */
  StringMap headers; /**< StringMap of HTTP headers. */
} *Response;


/** Construct a Request object by reading from a stream. */
Request Request_new(FILE *f);

/** Free a Request object. */
void Request_free(Request request);


/** Construct an HTTP response object. */
Response Response_new();

/** Add to a response's body. */
void Response_write(Response response, void *data, size_t size);

/** Print formatted text to a response's body. */
void Response_printf(Response response, const char *fmt, ...);

/** Send a response and then free it. */
void Response_send(Response response, FILE *f);

/** Set an HTTP header on a request or response object. */
#define Header_set(OBJECT, NAME, VALUE) StringMap_set(OBJECT->headers, NAME, (void*)strdup(VALUE))

/** Get an HTTP header on a request or response object. */
#define Header_get(OBJECT, NAME) ((char*) StringMap_get(OBJECT->headers, NAME))

