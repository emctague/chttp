#include "response.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct response make_response() {
  struct response resp;
  resp.status = 200;
  resp.data = NULL;
  resp.data_size = 0;
  resp.headers = NULL;
  return resp;
};

void append_response_body(struct response *response, char *data, size_t size) {
  response->data = realloc(response->data, response->data_size + size);
  memcpy(&response->data[response->data_size], data, size);
  response->data_size += size;
};

void send_and_free_response(struct response response, FILE *f) {
  fprintf(f, "HTTP/1.1 %d No Reason Phrase\r\n", response.status);
  fprintf(f, "Content-length: %zu\r\n", response.data_size);

  struct header *header = response.headers;

  while (header) {
    struct header *next = header->next;
    fprintf(f, "%s: %s\r\n", header->name, header->value);
    free(header->name);
    free(header->value);
    free(header);
    header = next;
  }

  fprintf(f, "\r\n");
  fwrite(response.data, response.data_size, 1, f);

  free(response.data);
}

void printf_response_body(struct response *response, const char *data, ...) {
  char *buf;
  va_list args;
  va_start(args, data);
  vasprintf(&buf, data, args);
  va_end(args);

  append_response_body(response, buf, strlen(buf));
  free(buf);
}
