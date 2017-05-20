CC=g++
CFLAGS= -Wall -g -std=c++11
ADDFLAGS= -DNDEBUG

default: Search

MapReduceFramework.a: MapReduceFramework.o
	ar rcs MapReduceFramework.a MapReduceFramework.o

MapReduceFramework.o: MapReduceFramework.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c MapReduceFramework.cpp

SearchMapReduce.o: SearchMapReduce.cpp FileNameKey.hpp MapReduceClient.h
	$(CC) $(CFLAGS) $(ADDFLAGS) -c SearchMapReduce.cpp FileNameKey.hpp MapReduceClient.h

Search: Search.cpp SearchMapReduce.o MapReduceFramework.a
	$(CC) $(CFLAGS) $(ADDFLAGS) SearchMapReduce.o Search.cpp MapReduceFramework.a -lstdc++fs -lpthread -o Search

tar:
	tar cvf ex3.tar FileNameKey.hpp *.cpp README Makefile

clean:
	rm -rf Search *.o *.a *.gch *.tar
