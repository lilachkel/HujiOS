CC = g++
CFLAGS = -Wall -g -std=c++11
ADDFLAGS = -DNDEBUG
SOURCES = CacheFS.cpp ICacheAlgorithm.hpp LruAlgorithm.cpp LfuAlgorithm.cpp FbrAlgorithm.cpp
HEADERS = LruAlgorithm.h LfuAlgorithm.h FbrAlgorithm.h
OBJECTS = CacheFS.o ICacheAlgorithm.hpp.gch LruAlgorithm.o LfuAlgorithm.o FbrAlgorithm.o

default: CacheFS.a

CacheFS.a: CacheFS.o
	ar rcs CacheFS.a $(OBJECTS)

CacheFS.o: $(SOURCES)
	$(CC) $(CFLAGS) $(ADDFLAGS) -c $(SOURCES)

tar:
	tar cvf ex4.tar $(SOURCES) $(HEADERS) README Answers.pdf Makefile

clean:
	rm -rf Search *.o *.a *.gch *.tar
