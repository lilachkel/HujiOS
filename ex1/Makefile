CC=g++
CFLAGS= -Wall -g -std=c++11

default: simpletest
	./simpletest

libosm.a: osm.o funcplaceholder.o
	ar rcs libosm.a osm.o funcplaceholder.o

osm.o: osm.cpp
	$(CC) $(CFLAGS) -c osm.cpp

funcplaceholder.o: funcplaceholder.cpp
	$(CC) $(CFLAGS) -c funcplaceholder.cpp

simpletest: simpletest.cpp libosm.a
	$(CC) $(CFLAGS) simpletest.cpp -L. -losm -o simpletest

tar:
	tar cvf ex1.tar  *.cpp *.h README Makefile

clean:
	rm -rf *.o *.a *.tar simpletest *.txt



