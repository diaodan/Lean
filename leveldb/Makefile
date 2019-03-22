CC = g++
CF = -std=c++0x  -g -lpthread
LIB= /usr/local/lib64/libleveldb.a

PRO=test

all:$(PRO)

test:test.cpp
	$(CC) $(CF) $^ $(LIB) -o $@

clean:
	rm -f $(PRO)

