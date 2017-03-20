CC=g++
CFLAGS= -Wall -g -std=c++11
ADDFLAGS= -DNDEBUG

default: libuthreads.a

libuthreads.a: uthreads.o
	ar rcs libuthreads.a uthreads.o

uthreads.o: uthreads.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c uthreads.cpp

tar:
	tar cvf ex2.tar  *.cpp README Makefile

clean:
	rm -rf *.o *.a *.tar
