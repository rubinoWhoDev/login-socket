CC=gcc
CFLAGS=-I.

all: server client

server:
	$(CC) server.c -o server $(CFLAGS)

client:
	$(CC) client.c -o client $(CFLAGS)

clean:
	rm -f server client
