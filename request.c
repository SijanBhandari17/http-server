#include "request.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int parseRequestLine(char *buffer, size_t n, Request *req) {

  char resLineBuffer[30];
  char lineArray[3][30];

  char *resLine = memmem(buffer, n, "\r\n", 2);
  if (resLine == NULL)
    return -1;

  size_t lineLen = (size_t)(resLine - buffer);
  if (lineLen >= sizeof(resLineBuffer))
    return -1;

  memcpy(resLineBuffer, buffer, lineLen);
  resLineBuffer[lineLen] = '\0';

  char *token = strtok(resLineBuffer, " ");
  int count = 0;

  while (token != NULL) {
    strcpy(lineArray[count++], token);
    token = strtok(NULL, " ");
  }
  strcpy(req->method, lineArray[0]);
  strcpy(req->path, lineArray[1]);
  strcpy(req->version, lineArray[2]);

  return 0;
}

int parseRequestHeaders(char *buffer, size_t n, Request *req) {

  char headerBuffer[200];
  char *resLine = memmem(buffer, n, "\r\n", 2);
  if (resLine == NULL)
    return -1;

  resLine += 2;

  size_t remaining = n - (size_t)(resLine - buffer);
  char *endLine = memmem(resLine, remaining, "\r\n\r\n", 4);

  if (endLine == NULL)
    return -1;

  size_t headerLen = (size_t)(endLine - resLine);

  if (headerLen >= sizeof(headerBuffer))
    return -1;

  memcpy(headerBuffer, resLine, headerLen);
  headerBuffer[headerLen] = '\0';

  char *token = strtok(headerBuffer, "\r\n");
  while (token != NULL) {
    strcat(req->headers, token);
    strcat(req->headers, "\n");
    token = strtok(NULL, "\r\n");
  }
  printf("Headers: %s", req->headers);
  return 0;
}

int parseRequest(int sockfd, int flags) {
  Request req = {0};
  char buffer[1024];

  ssize_t n = recv(sockfd, buffer, sizeof(buffer), flags);
  if (n > 0) {
    char *res = memmem(buffer, (size_t)n, "\r\n\r\n", 4);
    if (res == NULL)
      return -1;
    if (parseRequestLine(buffer, n, &req) == -1)
      return -1;
    if (parseRequestHeaders(buffer, n, &req) == -1)
      return -1;
  }
  return 0;
}
