#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>

#define MAX_REQUEST_BYTES 8192

typedef struct {
  char method[16];
  char path[2048];
  char version[16];
  char headers[8192];
} Request;

int parseRequestLine(char *buffer, size_t n, Request *req);

int parseRequestHeaders(char *buffer, size_t n, Request *req);

int parseRequest(int sockfd, int flags, Request *req);

#endif
