#include "request.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 10

int listing_fd(struct addrinfo *res) {

  struct sockaddr_storage addr;
  socklen_t addr_size = sizeof addr;

  int socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (socketfd == -1) {
    perror("socket");
    return -1;
  }

  int yes = 1;
  if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
    perror("setsockopt");
    return -1;
  }

  if (bind(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind");
    return -1;
  }

  if (listen(socketfd, BACKLOG) == -1) {
    perror("listen");
    return -1;
  }

  int new_fd = accept(socketfd, (struct sockaddr *)&addr, &addr_size);
  if (new_fd == -1) {
    perror("accept");
    return -1;
  }
  return new_fd;
}

int sendAll(int new_fd, const char *msg, size_t len, int flag) {

  size_t bytes_send = 0;
  while (bytes_send < len) {
    ssize_t n = send(new_fd, msg + bytes_send, len - bytes_send, flag);
    if (n == -1) {
      perror("send");
      return 0;
    }
    bytes_send += n;
  }
  return 1;
}

int main() {

  int rv;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE; // wildcard address

  hints.ai_family = AF_INET; // af_inet - ipv4 , af_inet6 - ipv6 , af_unspec -
                             // both as linked list

  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(NULL, "3649", &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo : %s\n", gai_strerror(rv));
    return 0;
  }

  for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
    int new_fd = listing_fd(p);

    if (new_fd == -1)
      continue;

    Request req = {0};
    parseRequest(new_fd, 0, &req);

    char *msg = "Hello, this is server";
    sendAll(new_fd, msg, strlen(msg), 0);
    if (close(new_fd) == -1) {
      perror("close");
      return 0;
    };
  }

  freeaddrinfo(res);
}
