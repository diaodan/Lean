CC = gcc
CF = -g
LIB = ../../hiredis/libhiredis.a

PRO = client
all:$(PRO)

client:main.c
	$(CC) $(CF) $^ $(LIB) -o $@

clean:
	rm -rf $(PRO)
