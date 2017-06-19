CC = g++
CFLAGS = -Wall -g -std=c++11
ADDFLAGS = -DNDEBUG
SOURCES = whatsappServer.cpp whatsappClient.cpp definitions.h

default: whatsappServer whatsappClient

whatsappServer: whatsappServer.cpp definitions.h
	$(CC) $(CFLAGS) $(ADDFLAGS) -c whatsappServer.cpp $(HEADERS) -o whatsappServer

whatsappClient: whatsappClient.cpp definitions.h
	$(CC) $(CFLAGS) $(ADDFLAGS) -c whatsappClient.cpp $(HEADERS) -o whatsappClient

tar:
	tar cvf ex5.tar $(SOURCES) README Makefile

clean:
	rm -rf whatsappClient whatsappServer *.o *.a *.gch *.tar
