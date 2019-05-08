CC = g++
CF = -g
LIB = -I../../log/ /usr/local/lib/libhiredis.a

SOURCE=../../log/ll_log.c net.c main.c

PRO = client
all:$(PRO)

client:$(SOURCE)
	$(CC) $(CF) $^ $(LIB) -o $@

clean:
	rm -rf $(PRO)
