#pragma once
#include <stdio.h>
#include <string.h>


/** HTTP Request Methods */
typedef enum HTTPMethod {
  HTTPMethodGET,
  HTTPMethodPOST,
  HTTPMethodHEAD,
  HTTPMethodUnknown
} HTTPMethod;

/** HTTP Header */
typedef struct Header *Header;

/** HTTP Request */
typedef struct Request {
  HTTPMethod method; /**< Request method. */
  char *path; /**< Request path. */
  Header headers; /**< Hash table of HTTP headers. */
  void *body; /**< Request body, if included. */
  size_t body_size; /**< Request body size. */
} *Request;


/** HTTP Response */
typedef struct Response {
  int status; /**< Response status code. */
  void *data; /**< Response data. */
  size_t data_size; /**< Response data size. */
  Header headers; /**< Hash table of HTTP headers. */
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

/** Update an HTTP Header on the given headers list. */
void Header_set(Header *object, char *name, char *value);

/** Get an HTTP Header on the given headers list. */
char *Header_get(Header *object, char *name);

