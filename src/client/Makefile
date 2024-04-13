CC ?=
CFLAGS ?=
MAINDIR ?=

OBJ_DIR := $(MAIN_DIR)/obj
INC_DIR := $(MAIN_DIR)/include
SRC_DIR := $(MAIN_DIR)/src
BIN_DIR := $(MAIN_DIR)/bin

SERV_OBJ_DIR := $(OBJ_DIR)/client
SERV_INC_DIR := $(INC_DIR)/client

COMM_OBJ_DIR := $(OBJ_DIR)/common
COMM_INC_DIR := $(INC_DIR)/common

TARGET := $(BIN_DIR)/client

.PHONY: all clean common_call

all: $(TARGET)

common_call: 
	@$(MAKE) -C $(MAIN_DIR) common

$(SERV_OBJ_DIR)/graphics.o: graphics.c \
	$(COMM_INC_DIR)/common.h \
	$(SERV_INC_DIR)/graphics.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SERV_OBJ_DIR)/threads.o: threads.c \
	$(SERV_INC_DIR)/threads.h \
	$(COMM_INC_DIR)/common.h \
	$(SERV_INC_DIR)/graphics.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SERV_OBJ_DIR)/client.o: client.c $(COMM_INC_DIR)/common.h $(SERV_INC_DIR)/graphics.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): \
	$(SERV_OBJ_DIR)/graphics.o \
	$(SERV_OBJ_DIR)/threads.o \
	$(SERV_OBJ_DIR)/client.o | common_call
	$(CC) $(CFLAGS) $^ $(wildcard $(COMM_OBJ_DIR)/*.o) -o $@ -lpthread -lrt -lncurses

clean:
	rm -f $(SERV_OBJ_DIR)/client.o
	rm -f $(SERV_OBJ_DIR)/graphics.o
	rm -f $(SERV_OBJ_DIR)/threads.o
	rm -f $(TARGET)