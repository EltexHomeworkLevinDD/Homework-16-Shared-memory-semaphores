CC ?=
CFLAGS ?=
MAINDIR ?=

TARGET := client

.PHONY: all clean

all: $(TARGET)

client.o: client.c ../common/common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): client.o ../common/common.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt

clean:
	rm -f client.o
	rm -f $(TARGET)