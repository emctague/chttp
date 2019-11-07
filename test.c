#include <chttp/CHTTP.h>

void handler(Request req, Response res) {
  Response_printf(res, "Hello!");
}

int main (int argc, char **argv) {
  Server server = Server_new();
  Server_setVerbosity(server, 3);
  Server_route(server, "/", handler);
  Server_route(server, "/oof", handler);
  Server_route(server, "/big_oof", handler);
  Server_listen(server, 8080);
}
