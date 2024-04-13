CC := gcc
CFLAGS := -Wall -Wextra -g
MAIN_DIR := $(CURDIR)

.PHONY: all common clean_common server clean_server client clean_client

all: server client

subdirs:
	@mkdir -p obj
	@mkdir -p obj/common
	@mkdir -p obj/server
	@mkdir -p obj/client
	@mkdir -p bin

common: | subdirs
	@$(MAKE) -C src/common CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

clean_common:
	@$(MAKE) -C src/common CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean

server: | common
	@$(MAKE) -C src/server CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

clean_server:
	@$(MAKE) -C src/server CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean

client: | common
	@$(MAKE) -C src/client CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) all

clean_client:
	@$(MAKE) -C src/client CC=$(CC) CFLAGS="$(CFLAGS)" MAIN_DIR=$(MAIN_DIR) clean

clean:
	rm -rf obj/
	rm -rf bin/