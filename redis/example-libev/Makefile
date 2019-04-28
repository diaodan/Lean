CC = gcc
CF = -g -lpthread -lev
LIB = /usr/local/lib/libhiredis.a

PRO = client
all:$(PRO)

client:main.c myredislib.c
	$(CC) $(CF) $^ $(LIB) -o $@

clean:
	rm -rf $(PRO)
