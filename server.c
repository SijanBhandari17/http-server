#include "request.h"
#include <netdb.h>
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
    return 1;
  }

  struct sockaddr_storage addr;
  socklen_t addr_size = sizeof addr;

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

  while (1) {
    addr_size = sizeof addr;
    int new_fd = accept(listener, (struct sockaddr *)&addr, &addr_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    Request req = {0};
    if (parseRequest(new_fd, 0, &req) == -1) {

      const char *resp = "HTTP/1.1 400 Bad Request\r\n"
                         "Content-Type: text/plain\r\n"
                         "Content-Length: 12\r\n"
                         "Connection: close\r\n"
                         "\r\n"
                         "Bad Request\n";
      sendAll(new_fd, resp, strlen(resp), 0);
    } else {
      const char *body = "Hello, this is server\n";
      char header[1024];
      int hlen = snprintf(header, sizeof header,
                          "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "Content-Length: %zu\r\n"
                          "Connection: close\r\n"
                          "\r\n",
                          strlen(body));
      sendAll(new_fd, header, (size_t)hlen, 0);
      sendAll(new_fd, body, strlen(body), 0);
    };

    if (close(new_fd) == -1) {
      perror("close");
    };
  }
  close(listener);
}
