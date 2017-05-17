CC=g++
CFLAGS= -Wall -g -std=c++11
ADDFLAGS= -DNDEBUG

default: Search

MapReduceFramework.a: MapReduceFramework.o Logger.o
	ar rcs MapReduceFramework.a MapReduceFramework.o Logger.o

MapReduceFramework.o: MapReduceFramework.cpp Logger.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c MapReduceFramework.cpp Logger.cpp

SearchMapReduce.o: SearchMapReduce.cpp FileNameKey.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c SearchMapReduce.cpp FileNameKey.cpp

Search: Search.cpp SearchMapReduce.o MapReduceFramework.a
	$(CC) $(CFLAGS) $(ADDFLAGS) SearchMapReduce.o FileNameKey.o Search.cpp MapReduceFramework.a -lstdc++fs -lpthread -o Search

tar:
	tar cvf ex2.tar FileNameKey.cpp *.cpp README Makefile

clean:
	rm -rf *.o *.a *.tar
