#include "response.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

const Response bad_req = {.status_line = "HTTP/1.1 400 Bad Request\r\n",
                          .headers = "Content-Type: text/plain\r\n"
                                     "Content-Length: 12\r\n"
                                     "Connection: close\r\n"
                                     "\r\n",
                          .message = "Bad Request\n"};

const Response not_found = {.status_line = "HTTP/1.1 404 Not Found\r\n",
                            .headers = "Content-Type: text/plain\r\n"
                                       "Content-Length: 17\r\n"
                                       "Connection: close\r\n"
                                       "\r\n",
                            .message = "Route not found \n"};

int sendResponse(int new_fd, const char *msg, size_t len, int flag) {

  size_t bytes_send = 0;
  while (bytes_send < len) {
    ssize_t n = send(new_fd, msg + bytes_send, len - bytes_send, flag);

    if (n == -1 && errno == EINTR) {
      continue;
    }

    if (n == -1) {
      perror("send");
      return 0;
    }
    bytes_send += n;
  }
  return 1;
}

int sendHttpResponse(int fd, const Response *res) {
  return sendResponse(fd, res->status_line, strlen(res->status_line), 0) &&
         sendResponse(fd, res->headers, strlen(res->headers), 0) &&
         sendResponse(fd, res->message, strlen(res->message), 0);
}
