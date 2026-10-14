#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>

typedef struct {
  char method[16];
  char path[256];
  char version[16];
  char headers[1024];
} Request;

int parseRequestLine(char *buffer, size_t n, Request *req);

int parseRequestHeaders(char *buffer, size_t n, Request *req);

int parseRequest(int sockfd, int flags);

#endif
