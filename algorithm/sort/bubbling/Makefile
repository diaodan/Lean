CC = gcc
CFLAGS = 


PRO=sort sort1 sort2 sort3 gsort1 gsort2 gsort3

all:$(PRO)

gsort1:bubbling.c
	$(CC) $^ $(CFLAGS) -g -o $@
gsort2:bubbling.c
	$(CC) $^ $(CFLAGS) -g2 -o $@
gsort3:bubbling.c
	$(CC) $^ $(CFLAGS) -g3 -o $@
sort:bubbling.c
	$(CC) $^ $(CFLAGS) -o $@

sort1:bubbling.c
	$(CC) $^ $(CFLAGS) -O -o $@

sort2:bubbling.c
	$(CC) $^ $(CFLAGS) -O2 -o $@

sort3:bubbling.c
	$(CC) $^ $(CFLAGS) -O3 -o $@

clean:
	rm -rf $(PRO)
