CC=g++
CFLAGS= -Wall -g -std=c++11
ADDFLAGS= -DNDEBUG

default: libosm.a

all: simpletest
	mkdir /tmp/tik
	touch /tmp/tik/someKovez
	./simpletest

libosm.a: osm.o funcplaceholder.o
	ar rcs libosm.a osm.o funcplaceholder.o

osm.o: osm.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c osm.cpp

funcplaceholder.o: funcplaceholder.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c funcplaceholder.cpp

simpletest: simpletest.cpp libosm.a
	$(CC) $(CFLAGS) $(ADDFLAGS) simpletest.cpp -L. -losm -o simpletest

tar:
	tar cvf ex1.tar  *.cpp funcplaceholder.h README Makefile

clean:
	rm -rf *.o *.a *.tar simpletest *.txt /tmp/tik



