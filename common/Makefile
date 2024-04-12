CC ?=
CFLAGS ?=
MAINDIR ?=

.PHONY: all clean

all: common.o

common.o: common.c common.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f common.o