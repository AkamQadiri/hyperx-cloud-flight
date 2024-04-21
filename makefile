CC := gcc
CFLAGS := -Wall -Werror -Wextra -Iinclude
LDFLAGS := -lhidapi-hidraw

SRC_DIR := src
OBJ_DIR := obj
BUILD_DIR := /usr/local/bin

EXECUTABLE_NAME := cloud_flight
UDEV_RULES_FILE := /etc/udev/rules.d/99-hyperx-cloud-flight.rules

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

.PHONY: all clean uninstall install

all: install

$(BUILD_DIR)/$(EXECUTABLE_NAME): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

install: $(BUILD_DIR)/$(EXECUTABLE_NAME)
	@echo "Installing $(EXECUTABLE_NAME) to $(BUILD_DIR)"
	echo 'KERNEL=="hidraw*", ATTRS{idVendor}=="0951", ATTRS{idProduct}=="16c4", MODE="0666"' | sudo tee $(UDEV_RULES_FILE) > /dev/null
	echo 'KERNEL=="hidraw*", ATTRS{idVendor}=="0951", ATTRS{idProduct}=="1723", MODE="0666"' | sudo tee -a $(UDEV_RULES_FILE) > /dev/null
	udevadm control --reload-rules && udevadm trigger

clean:
	rm -rf $(OBJ_DIR)

uninstall:
	rm -f $(BUILD_DIR)/$(EXECUTABLE_NAME)
	rm -f $(UDEV_RULES_FILE)
