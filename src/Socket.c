#include <chttp/Socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>

struct Socket {
  int fd; /**< Socket file descriptor. */
  struct sockaddr_in address; /**< Server address. */
  socklen_t addr_len; /**< Length of a sockaddr_in. */
  int connCounter; /**< Connection counter. */
};

Socket Socket_new (int port) {
  Socket sock = malloc(sizeof(struct Socket));
  sock->addr_len = sizeof(struct sockaddr_in);
  sock->fd = socket(AF_INET, SOCK_STREAM, 0);
  sock->connCounter = 0;
  /* TODO: Handle error if socket->fd is 0 */

  /* Ensure address is usable after this socket stops using it. */
  int s_true = 1;
  setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &s_true, sizeof(int));

  bzero(&sock->address, sock->addr_len);
  sock->address.sin_family = AF_INET;
  sock->address.sin_addr.s_addr = htonl(INADDR_ANY);
  sock->address.sin_port = htons(port);

  bind(sock->fd, (struct sockaddr*)&sock->address, sock->addr_len);
  /* TODO: Handle error if bind result is not 0. */

  listen(sock->fd, 5);
  /* TODO: Handle error if listen result is not 0. */

  return sock;
}


void Socket_free (Socket socket) {
  close(socket->fd);
  free(socket);
}


struct SocketAcceptData {
  PFNSocketConnHandler connHandler;
  int fd;
  struct sockaddr_in cli;
  void *userPointer;
  int uniqueID;
};

void *Socket_handleAccept (struct SocketAcceptData *data) {
  PFNSocketConnHandler connHandler = data->connHandler;
  int fd = data->fd;
  struct sockaddr_in cli = data->cli;
  void *userPointer = data->userPointer;
  int uniqueID = data->uniqueID;
  char *client_address = NULL;
  FILE *io;

  free(data);

  /* Get IP address as a string. */
  size_t ip_len = 1 + cli.sin_family == AF_INET6 ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN;
  client_address = malloc(ip_len);
  inet_ntop(cli.sin_family, &cli.sin_addr, client_address, ip_len);
  client_address[ip_len - 1] = '\0';

  /* Open fd and handle connection. */
  io = fdopen(fd, "r+");
  connHandler(io, client_address, uniqueID, userPointer);

  fclose(io);
  free(client_address);
  close(fd);
  pthread_exit(NULL);
}


int Socket_accept (Socket socket, PFNSocketConnHandler connHandler, PFNSocketErrHandler errHandler, void *userPointer) {
  int fd;
  char *client_address = NULL;
  FILE *io;
  struct sockaddr_in cli;

  fd = accept(socket->fd, (struct sockaddr*)&cli, &socket->addr_len);
  if (fd < 1) return errHandler(errno, socket->connCounter++, userPointer);

  struct SocketAcceptData *data = malloc(sizeof(struct SocketAcceptData));
  data->fd = fd;
  data->cli = cli;
  data->connHandler = connHandler;
  data->userPointer = userPointer;
  data->uniqueID = socket->connCounter++;

  pthread_t thread;
  pthread_create(&thread, NULL, (void*(*)(void*))Socket_handleAccept, (void*)data);

  return 1;
}

