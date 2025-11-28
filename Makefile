BIN_PATH=./bin

CFLAGS= -Wall -Wextra -I./lib

LOG_LEVEL ?= 3

build-examples:
	cc $(CFLAGS) -DLOG_LEVEL=$(LOG_LEVEL) -o $(BIN_PATH)/examples examples/main.c

run-examples: build-examples
	$(BIN_PATH)/examples

clean:
	find $(BIN_PATH) -mindepth 1 ! -name '.gitkeep' -exec rm -rf {} +
