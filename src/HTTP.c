#include <chttp/HTTP.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

Request Request_new(FILE *f) {
  Request request = malloc(sizeof(struct Request));
  request->headers = StringMap_new(free);
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
    Header_set(request, name, value);
  }

  for (int i = 0; i < StringMap_size(request->headers); i++) {
    printf("%s\n", StringMap_getNameAt(request->headers, i));
  }

  /* Read body, if applicable. */
  char *clength = Header_get(request, "content-length");
  if (clength) {
    printf("Clength: %s\n", clength);
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
  StringMap_free(request->headers);
}

Response Response_new() {
  Response response = malloc(sizeof(struct Response));
  response->status = 200;
  response->data = NULL;
  response->data_size = 0;
  response->headers = StringMap_new(free);
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

  for (int i = 0; i < StringMap_size(response->headers); i++) {
    char *name = StringMap_getNameAt(response->headers, i);
    char *value = (char*)StringMap_getValueAt(response->headers, i);
    fprintf(f, "%s: %s\r\n", name, value);
  }

  fprintf(f, "\r\n");
  fwrite(response->data, response->data_size, 1, f);

  free(response->data);
  StringMap_free(response->headers);
  free(response);
}

