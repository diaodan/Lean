CC = gcc
CFLAGS = -g3 -Wall

all:client server

PRO=client server

client:client.c
	$(CC) $^ $(CFLAGS) -o $@

server:server.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm -rf $(PRO)
