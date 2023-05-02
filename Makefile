CC=gcc
CFLAGS=-Wall -Wextra

OBJS=server.o net.o file.o

file.o: file.c file.h

net.o: net.c net.h

server.o: server.c net.h file.h 

server: $(OBJS)
	gcc -o $@ $^ -g

clean:
	rm -f $(OBJS)
	rm -f server

all: server

.PHONY: all, clean