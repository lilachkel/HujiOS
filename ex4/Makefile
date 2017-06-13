CC=g++
CFLAGS= -Wall -g -std=c++11
ADDFLAGS= -DNDEBUG
SOURCES= CacheFS.cpp ICacheAlgorithm.hpp LruAlgorithm.cpp LfuAlgorithm.cpp FbrAlgorithm.cpp
HEADERS= LruAlgorithm.h LfuAlgorithm.h FbrAlgorithm.h

default: CacheFS.a TestLFU

CacheFS.a: CacheFS.o
	ar rcs CacheFS.a CacheFS.o

CacheFS.o: $(SOURCES)
	$(CC) $(CFLAGS) $(ADDFLAGS) -c $(SOURCES)

TestLFU: TestLFU.cpp CacheFS.a
	$(CC) $(CFLAGS) $(ADDFLAGS) TestLFU.cpp CacheFS.a -o TestLFU

tar:
	tar cvf ex4.tar $(SOURCES) $(HEADERS) README Makefile

clean:
	rm -rf Search *.o *.a *.gch *.tar
