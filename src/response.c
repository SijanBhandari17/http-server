#include "response.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

const Response bad_req = {.status_line = "HTTP/1.1 400 Bad Request\r\n",
                          .headers =
                              "Content-Type: text/html; charset=UTF-8\r\n"
                              "Connection: close\r\n"
                              "\r\n",
                          .message = "assets/400.html"};

const Response not_found = {.status_line = "HTTP/1.1 404 Not Found\r\n",
                            .headers =
                                "Content-Type: text/html; charset=UTF-8\r\n"
                                "Connection: close\r\n"
                                "\r\n",
                            .message = "assets/404.html"};

const Response hello_response = {
    .status_line = "HTTP/1.1 200 OK\r\n",
    .headers = "Content-Type: text/html; charset=UTF-8\r\n"
               "charset=UTF-8\r\n"
               "Connection: close\r\n"
               "\r\n",
    .message = "assets/hello.html"};

const Response index_response = {
    .status_line = "HTTP/1.1 200 OK\r\n",
    .headers = "Content-Type: text/html; charset=UTF-8\r\n"
               "Connection: close\r\n"
               "\r\n",
    .message = "assets/index.html"};

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

int sendHTML(int fd, const char *file, int flag) {
  FILE *html = fopen(file, "rb");

  if (html == NULL) {
    perror("fopen");
    return 0;
  }

  char buf[1024];
  size_t bytes_read;

  while ((bytes_read = fread(buf, 1, sizeof(buf), html)) > 0) {
    if (!sendResponse(fd, buf, bytes_read, flag)) {
      fclose(html);
      return 0;
    }
  }

  if (ferror(html)) {
    perror("fread");
    fclose(html);
    return 0;
  }

  fclose(html);
  return 1;
}

int sendHttpResponse(int fd, const Response *res) {
  return sendResponse(fd, res->status_line, strlen(res->status_line), 0) &&
         sendResponse(fd, res->headers, strlen(res->headers), 0) &&
         sendHTML(fd, res->message, 0);
}
