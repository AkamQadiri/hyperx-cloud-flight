CC := gcc
CFLAGS := -O2 -Wall -Werror -Wextra -Iinclude -MMD -MP
LDFLAGS := -lhidapi-hidraw

PREFIX ?= /usr/local

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

EXECUTABLE_NAME := cloud_flight
UDEV_RULES_SRC := udev/99-hyperx-cloud-flight.rules
UDEV_RULES_NAME := $(notdir $(UDEV_RULES_SRC))

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
DEP := $(OBJ:.o=.d)

.PHONY: all clean install uninstall

all: $(BIN_DIR)/$(EXECUTABLE_NAME)

$(BIN_DIR)/$(EXECUTABLE_NAME): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

install: all
	install -Dm755 $(BIN_DIR)/$(EXECUTABLE_NAME) $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE_NAME)
	install -Dm644 $(UDEV_RULES_SRC) $(DESTDIR)/etc/udev/rules.d/$(UDEV_RULES_NAME)
	[ -n "$(DESTDIR)" ] || { udevadm control --reload-rules && udevadm trigger; }

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE_NAME)
	rm -f $(DESTDIR)/etc/udev/rules.d/$(UDEV_RULES_NAME)
	[ -n "$(DESTDIR)" ] || udevadm control --reload-rules

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
