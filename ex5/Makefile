CC = g++
CFLAGS = -Wall -g -std=c++11
ADDFLAGS = -DNDEBUG
HEADERS = definitions.h NetworkHandler.h
SOURCES = whatsappServer.cpp whatsappClient.cpp NetworkHandler.cpp

default: whatsappServer whatsappClient

whatsappServer: whatsappServer.cpp NetworkHandler.o
	$(CC) $(CFLAGS) $(ADDFLAGS) NetworkHandler.o -c whatsappServer.cpp -o whatsappServer

whatsappClient: whatsappClient.cpp NetworkHandler.o
	$(CC) $(CFLAGS) $(ADDFLAGS) NetworkHandler.o -c whatsappClient.cpp -o whatsappClient

NetworkHandler.o: NetworkHandler.cpp definitions.h
	$(CC) $(CFLAGS) $(ADDFLAGS) -c definitions.h NetworkHandler.cpp

tar:
	tar cvf ex5.tar $(SOURCES) README Makefile

clean:
	rm -rf whatsappClient whatsappServer *.o *.a *.gch *.tar
