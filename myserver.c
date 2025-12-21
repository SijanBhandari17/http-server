#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define BACKLOG 10

int main() {

  int status, rv;
  struct addrinfo hints, *res;
  struct sockaddr_storage addr;
  socklen_t addr_size;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE; // wildcard address

  hints.ai_family = AF_INET; // af_inet - ipv4 , af_inet6 - ipv6 , af_unspec -
                             // both as linked list

  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(NULL, "3649", &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo : %s\n", gai_strerror(rv));
    return 0;
  }

  int socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (socketfd == -1) {
    perror("socket");
    return 0;
  }

  int yes = 1;
  if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
    perror("setsockopt");
    return 0;
  }

  if (bind(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind");
    return 0;
  }

  if (listen(socketfd, BACKLOG) == -1) {
    perror("listen");
    return 0;
  }

  addr_size = sizeof addr;
  int new_fd = accept(socketfd, (struct sockaddr *)&addr, &addr_size);

  char *msg = "Hello";

  int bytes_send = send(new_fd, msg, strlen(msg), 0);
  addr_size = sizeof(addr);

  if (getpeername(new_fd, (struct sockaddr *)&addr, &addr_size) == -1) {
    perror("getpeername");
  }

  freeaddrinfo(res);
}
