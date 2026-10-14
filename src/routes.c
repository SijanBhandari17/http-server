#include "routes.h"
#include "response.h"
#include <string.h>

const Routes routes[] = {
    {.method = "GET", .path = "/", .response = &index_response},
    {.method = "GET", .path = "/index", .response = &index_response},
    {.method = "GET", .path = "/index.html", .response = &index_response},

    {.method = "GET", .path = "/hello", .response = &hello_response},
    {.method = "GET", .path = "/hello.html", .response = &hello_response},

    {.method = "GET", .path = "/400.html", .response = &bad_req},
};

const size_t route_count = sizeof(routes) / sizeof(routes[0]);

const Response *match_routes(const Routes *routes, const char *method,
                             const char *path, size_t route_count) {
  for (size_t i = 0; i < route_count; i++) {
    if (strcmp(routes[i].method, method) == 0 &&
        strcmp(routes[i].path, path) == 0) {
      return routes[i].response;
    }
  }

  return &not_found;
}
