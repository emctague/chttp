#include "socket.h"
#include "error.h"

#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>

struct smart_socket new_sock(int port)
{
	struct smart_socket sock;
	sock.len = sizeof(struct sockaddr_in);
	
	sock.sock = socket(AF_INET, SOCK_STREAM, 0);
	doe_n0(!sock.sock, RS_CANNOT_MAKE_SOCKET);

	int s_true = 1;
	setsockopt(sock.sock, SOL_SOCKET, SO_REUSEADDR, &s_true, sizeof(int));
	
	bzero(&sock.addr, sock.len);
	sock.addr.sin_family = AF_INET;
	sock.addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sock.addr.sin_port = htons(port);

	doe_n0(bind(sock.sock, (struct sockaddr*)&sock.addr, sock.len), RS_CANNOT_BIND, port);
	doe_n0(listen(sock.sock, 5), RS_CANNOT_LISTEN);

	return sock;
}

void free_sock(struct smart_socket sock) {
	close(sock.sock);	
}

int accept_conn(struct smart_socket *socket, pfn_conn_handler handler, pfn_err_handler err_handler) {
	int conn;
	char *client_address = NULL;
	FILE *io;

	struct sockaddr_in cli;
	conn = accept(socket->sock, (struct sockaddr*)&cli, &socket->len);
	if (conn < 1) return err_handler(errno);

	/* Get IP address as string. */
	size_t ip_len = 1;
	if (cli.sin_family == AF_INET6) ip_len += INET6_ADDRSTRLEN;
	else ip_len += INET_ADDRSTRLEN;

	client_address = malloc(ip_len);
	inet_ntop(cli.sin_family, &cli.sin_addr, client_address, ip_len);
	client_address[ip_len - 1] = '\0';

	io = fdopen(conn, "r+");
	handler(io, client_address);
	free(client_address);
	fclose(io);
	close(conn);

	return 1;
}

