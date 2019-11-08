#include <chttp/CHTTP.h>

Result handler(Request req, Response res) {
  printf("Header: %s\n", Header_get(&req->headers, "user-agent"));
  Response_printf(res, "Request body: (%d) '%s'\n", req->body_size, (char*)req->body);
  return ResultOK(NULL);
}

Result handler404(Request req, Response res) {
  Header_set(&res->headers, "content-type", "text/html");
  Response_printf(res, "<h1>Error 404 - Not Found</h1>");
  return ResultOK(NULL);
}

int main (int argc, char **argv) {
  Server server = Server_new();
  Server_setVerbosity(server, 3);
  Server_route(server, "/", handler);
  Server_route(server, "/oof", handler);
  Server_route(server, "/aaf", handler);
  Server_route404(server, handler404); 
  OkOr(void*, (Server_listen(server, 8080)), {
    fprintf(stderr, "%s\n", OkOrMessage);
    return 1;
  });
}
