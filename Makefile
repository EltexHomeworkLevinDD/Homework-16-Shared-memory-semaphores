CC := gcc
CFLAGS := -Wall -Wextra -g
MAINDIR := $(CURDIR)

.PHONY: all common clean_common server clean_server client clean_client

all: server client

common:
	@$(MAKE) -C common CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

clean_common:
	@$(MAKE) -C common CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean

server: | common
	@$(MAKE) -C server CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

clean_server:
	@$(MAKE) -C server CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean

client: | common
	@$(MAKE) -C client CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

clean_client:
	@$(MAKE) -C client CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean

clean: clean_common clean_server clean_client
