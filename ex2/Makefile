CC=g++
CFLAGS= -Wall -g -std=c++11
ADDFLAGS= -DNDEBUG

default: libuthreads.a

libuthreads.a: uthreads.o Thread.o
	ar rcs libuthreads.a uthreads.o Thread.o

uthreads.o: uthreads.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c uthreads.cpp

Thread.o: Thread.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c Thread.cpp

tar:
	tar cvf ex2.tar  *.cpp README Makefile

clean:
	rm -rf *.o *.a *.tar
