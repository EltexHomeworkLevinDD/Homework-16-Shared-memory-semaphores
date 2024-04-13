CC ?=
CFLAGS ?=
MAINDIR ?=

OBJ_DIR := $(MAIN_DIR)/obj
INC_DIR := $(MAIN_DIR)/include
SRC_DIR := $(MAIN_DIR)/src

.PHONY: all clean

all: $(OBJ_DIR)/common/common.o

$(OBJ_DIR)/common/common.o: common.c $(INC_DIR)/common/common.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/common/common.o