#ifndef RESPONSE_H
#define RESPONSE_H

#include <stddef.h>
typedef struct {
  char status_line[64];
  char headers[256];
  char message[128];
} Response;

extern const Response bad_req;
extern const Response not_found;

int sendResponse(int fd, const char *msg, size_t len, int flag);
int sendHttpResponse(int fd, const Response *res);

#endif
