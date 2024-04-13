CC ?=
CFLAGS ?=
MAINDIR ?=

OBJ_DIR := $(MAIN_DIR)/obj
INC_DIR := $(MAIN_DIR)/include
SRC_DIR := $(MAIN_DIR)/src
BIN_DIR := $(MAIN_DIR)/bin

SERV_OBJ_DIR := $(OBJ_DIR)/server
SERV_INC_DIR := $(INC_DIR)/server

COMM_OBJ_DIR := $(OBJ_DIR)/common
COMM_INC_DIR := $(INC_DIR)/common

TARGET := $(BIN_DIR)/server

.PHONY: all clean common_call

all: $(TARGET)

common_call: 
	@$(MAKE) -C $(MAIN_DIR) common

$(SERV_OBJ_DIR)/server.o: server.c $(COMM_INC_DIR)/common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(SERV_OBJ_DIR)/server.o | common_call
	$(CC) $(CFLAGS) $^ $(wildcard $(COMM_OBJ_DIR)/*.o) -o $@ -lpthread -lrt

clean:
	rm -f $(SERV_OBJ_DIR)/server.o
	rm -f $(TARGET)