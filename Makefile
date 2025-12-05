day       ?= 1
log_level ?= 3
part      ?= 1

SRC_DIR   := day$(day)
BIN       := ./bin/day$(day)
SRC       := $(SRC_DIR)/main.c
INCLUDES  := -I./lib

CFLAGS    := $(INCLUDES) -DLOG_LEVEL=$(log_level) 
ifeq ($(part),2)
CFLAGS += -DPART2
endif

CC        := cc

.PHONY: build run clean

build:
	@echo "Building Day $(day) with LOG_LEVEL=$(log_level)"
	@mkdir -p bin
	$(CC) $(SRC) $(CFLAGS) -o $(BIN)
	@echo "Executable stored at $(BIN)"

run:
	@echo "Running Day $(day)..."
	$(BIN)

clean:
	@echo "Cleaning binaries..."
	rm -rf bin
