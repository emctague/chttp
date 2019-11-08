#define _WITH_GETLINE
#include <chttp/HTTP.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include "thirdparty/uthash.h"

struct Header {
  char *name;
  char *value;
  UT_hash_handle hh;
};

Request Request_new(FILE *f) {
  Request request = malloc(sizeof(struct Request));
  request->headers = NULL;
  request->body_size = 0;
  request->body = NULL;

  char *line = NULL;
  size_t read = 0;

  /* Get request type. */
  getdelim(&line, &read, ' ', f);
  if (!strcmp(line, "GET ")) request->method = HTTPMethodGET;
  else if (!strcmp(line, "POST ")) request->method = HTTPMethodPOST;
  else if (!strcmp(line, "HEAD ")) request->method = HTTPMethodHEAD;
  else request->method = HTTPMethodUnknown;

  /* Get request path. */
  size_t path_len = getdelim(&line, &read, ' ', f);
  line[path_len - 1] = '\0';

  /* Parse percent-encoded characters. */
  char *path_parsed = malloc(path_len);
  int out_i = 0;
  for (int i = 0; i < path_len - 1; i++) {
    if (line[i] == '%') {
      char text[3] = { line[i + 1], line[i + 2], 0};
      path_parsed[out_i] = (char)strtol(text, NULL, 16);
      i += 2;
    } else {
      path_parsed[out_i] = line[i];
    }
    out_i++;
  }
  path_parsed[out_i] = '\0';


  /* We strdup in order to clear up excess memory. */
  request->path = strdup(path_parsed);
  free(path_parsed);

  /* Dispose of the rest of the line because the HTTP version is irrelevant to us. */
  getline(&line, &read, f);

  /* Get headers. */
  while (getline(&line, &read, f)) {
    if (!strcmp(line, "\r\n")) break;
    char *name = strtok(line, ": ");
    char *value = strtok(NULL, "\r\n");

    // Make headers lowercase
    int namelen = strlen(name);
    for (int i = 0; i < namelen; i++) {
      name[i] = tolower(name[i]);
    }

    Header_set(&request->headers, name, value);
  }

  /* Read body, if applicable. */
  char *clength = Header_get(&request->headers, "content-length");
  if (clength) {
    request->body_size = atoi(clength);
    request->body = malloc(request->body_size + 1); // 1 extra byte for a convenience null terminator
    fread(request->body, request->body_size, 1, f); 
    ((char*)request->body)[request->body_size] = '\0';
  }

  free(line);
  return request;
}

void Request_free(Request request) {
  free(request->path);

  Header r, tmp;
  HASH_ITER(hh, request->headers, r, tmp) {
    HASH_DEL(request->headers, r);
    free(r->name);
    free(r->value);
    free(r);
  }
}

Response Response_new() {
  Response response = malloc(sizeof(struct Response));
  response->status = 200;
  response->data = NULL;
  response->data_size = 0;
  response->headers = NULL;
  return response;
}

void Response_write(Response response, void *data, size_t size) {
  response->data = realloc(response->data, response->data_size + size);
  memcpy(&response->data[response->data_size], data, size);
  response->data_size += size;
}

void Response_printf(Response response, const char *fmt, ...) {
  char *buf;
  va_list args;
  va_start(args, fmt);
  vasprintf(&buf, fmt, args);
  va_end(args);
  Response_write(response, buf, strlen(buf));
  free(buf);
}

void Response_send(Response response, FILE *f) {
  fprintf(f, "HTTP/1.1 %d No Reason\r\n", response->status);
  fprintf(f, "Content-length: %zu\r\n", response->data_size);

  Header r, tmp;
  HASH_ITER(hh, response->headers, r, tmp) {
    fprintf(f, "%s: %s\r\n", r->name, r->value);
    HASH_DEL(response->headers, r);
    free(r->name);
    free(r->value);
    free(r);
  }

  fprintf(f, "\r\n");
  fwrite(response->data, response->data_size, 1, f);

  free(response->data);
  free(response);
}

char *Header_get(Header *object, char *name) {
  Header r = NULL;
  HASH_FIND_STR(*object, name, r);
  if (r) return r->value;
  else return NULL;
}

void Header_set(Header *object, char *name, char *value) {
  Header r = NULL;
  HASH_FIND_STR(*object, name, r);
  if (r) {
    free(r->value);
    r->value = strdup(value);
  } else {
    r = malloc(sizeof(struct Header));
    r->name = strdup(name);
    r->value = strdup(value);
    HASH_ADD_KEYPTR(hh, *object, r->name, strlen(r->name), r);
  }
}

