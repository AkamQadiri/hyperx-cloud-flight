CC := gcc
CFLAGS := -Wall -Werror -Wextra -Iinclude
LDFLAGS := -lhidapi-hidraw

SRC_DIR := src
OBJ_DIR := obj
BUILD_DIR := bin

EXECUTABLE_NAME := cloud_flight

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

.PHONY: all clean

all: $(BUILD_DIR)/$(EXECUTABLE_NAME)

$(BUILD_DIR)/$(EXECUTABLE_NAME): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)
