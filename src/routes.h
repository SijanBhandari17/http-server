#ifndef ROUTES_H
#define ROUTES_H

#include "response.h"
typedef struct {
  char method[16];
  char path[2048];
  const Response *response;
} Routes;

extern const Routes routes[];
extern const size_t route_count;

const Response *match_routes(const Routes *routes, const char *method,
                             const char *path, size_t route_count);

#endif
