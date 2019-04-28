CC = gcc
CF = -g
LIB = -I../../log/ /usr/local/lib/libhiredis.a

SOURCE=main.c ../../log/ll_log.c net.c

PRO = client
all:$(PRO)

client:$(SOURCE)
	$(CC) $(CF) $^ $(LIB) -o $@

clean:
	rm -rf $(PRO)
