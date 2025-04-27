CC := g++
CFLAGS := -O0 -g -Wextra -Wall -Wno-write-strings -Wno-unused-function -Wno-unused-variable
DEFINES := -D_DEBUG=1
LIBS := -luser32 -lgdi32

RES_DIR := res
SRC_DIR := src
BUILD_DIR := build

EXEC := snake.exe

.PHONY: all
all: $(BUILD_DIR) $(BUILD_DIR)/$(EXEC)

$(BUILD_DIR):
	mkdir $@

$(BUILD_DIR)/$(EXEC): $(SRC_DIR)/snake.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(DEFINES) $(LIBS)

.PHONY: run
run: all
	$(BUILD_DIR)/$(EXEC)

.PHONY: clean
clean:
	del /q $(BUILD_DIR)\*.*
