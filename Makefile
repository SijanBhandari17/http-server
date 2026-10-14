CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g -Isrc

BUILD_DIR = build

SERVER_SOURCES = src/server.c \
                 src/request.c \
                 src/response.c \
                 src/routes.c

SERVER = $(BUILD_DIR)/server
TEST_REQUEST = $(BUILD_DIR)/test_request

.PHONY:	all test clean

all: $(SERVER)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SERVER): $(SERVER_SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SERVER_SOURCES) -o $(SERVER)

$(TEST_REQUEST): tests/test_request.c src/request.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) tests/test_request.c src/request.c -o $(TEST_REQUEST)

test: $(TEST_REQUEST)
	./$(TEST_REQUEST)

clean:
	rm -rf $(BUILD_DIR)
