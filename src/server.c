#include "request.h"
#include "response.h"
#include "routes.h"
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 10

int create_listener(struct addrinfo *res) {

  int socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (socketfd == -1) {
    close(socketfd);
    perror("socket");
    return -1;
  }

  int yes = 1;
  if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
    close(socketfd);
    perror("setsockopt");
    return -1;
  }

  if (bind(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
    close(socketfd);
    perror("bind");
    return -1;
  }

  if (listen(socketfd, BACKLOG) == -1) {
    close(socketfd);
    perror("listen");
    return -1;
  }
  return socketfd;
}

void acceptConnection(struct sockaddr_storage addr, int listener) {

  while (1) {
    socklen_t addr_size = sizeof addr;
    int new_fd = accept(listener, (struct sockaddr *)&addr, &addr_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    Request req = {0};

    if (parseRequest(new_fd, 0, &req) == -1) {
      sendHttpResponse(new_fd, &bad_req);
    } else {
      const Response *res =
          match_routes(routes, req.method, req.path, route_count);
      sendHttpResponse(new_fd, res);
    };

    if (close(new_fd) == -1) {
      perror("close");
    };
  }
}

int main() {

  signal(SIGPIPE, SIG_IGN);

  int rv;
  struct addrinfo hints, *res;
  struct sockaddr_storage addr;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE; // wildcard address

  hints.ai_family = AF_INET; // af_inet - ipv4 , af_inet6 - ipv6 , af_unspec -
                             // both as linked list

  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(NULL, "3649", &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo : %s\n", gai_strerror(rv));
    return 1;
  }

  int listener = -1;
  for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
    listener = create_listener(p);
    if (listener != -1)
      break;
  }
  freeaddrinfo(res);

  if (listener == -1) {
    fprintf(stderr, "failed to create listener\n");
    return 1;
  }
  acceptConnection(addr, listener);

  close(listener);
}
