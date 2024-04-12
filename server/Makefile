CC ?=
CFLAGS ?=
MAINDIR ?=

TARGET := server

.PHONY: all clean

all: $(TARGET)

server.o: server.c ../common/common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): server.o ../common/common.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f server.o
	rm -f $(TARGET)