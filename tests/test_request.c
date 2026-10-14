#include "../request.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *name;
  const char *request;
  int expect_line_ok;
  int expect_headers_ok;
} TestCase;

int main(void) {
  TestCase tests[] = {
      {"Valid HTTP/1.1 with one header", "GET / HTTP/1.1\r\nHost: x\r\n\r\n", 1,
       1},
      {"Valid HTTP/1.0 with zero headers", "GET / HTTP/1.0\r\n\r\n", 1, 1},
      {"Too few request-line tokens", "GET /\r\n\r\n", 0, 0},
      {"Too many request-line tokens", "GET / HTTP/1.1 extra\r\n\r\n", 0, 0},
      {"Header missing colon", "GET / HTTP/1.1\r\nHost x\r\n\r\n", 1, 0},
      {"Incomplete headers", "GET / HTTP/1.1\r\nHost: x\r\n", 1, 0}};

  size_t count = sizeof(tests) / sizeof(tests[0]);

  for (size_t i = 0; i < count; i++) {
    Request req = {0};
    size_t n = strlen(tests[i].request);

    int line_result = parseRequestLine((char *)tests[i].request, n, &req);

    /*
     * Only call the header parser if the request line
     * parsed successfully. Header parsing is tested separately
     * after request-line validation.
     */
    int headers_result = -1;

    if (line_result == 0) {
      headers_result = parseRequestHeaders((char *)tests[i].request, n, &req);
    }

    int line_ok = (line_result == 0);
    int headers_ok = (headers_result == 0);

    printf("[%s] %s\n",
           line_ok == tests[i].expect_line_ok &&
                   headers_ok == tests[i].expect_headers_ok
               ? "PASS"
               : "FAIL",
           tests[i].name);

    printf("  Request line: %s\n", line_ok ? "accepted" : "rejected");

    printf("  Headers: %s\n", headers_ok ? "accepted" : "rejected");
  }

  return 0;
}
