CC = g++
CFLAGS = -Wall -g -std=c++11
ADDFLAGS = -DNDEBUG
HEADERS = definitions.h NetworkHandler.h
SOURCES = whatsappServer.cpp whatsappClient.cpp NetworkHandler.cpp

default: whatsappServer whatsappClient

whatsappServer: whatsappServer.cpp NetworkHandler.o
	$(CC) $(CFLAGS) $(ADDFLAGS) NetworkHandler.o whatsappServer.cpp -o whatsappServer

whatsappClient: whatsappClient.cpp NetworkHandler.o
	$(CC) $(CFLAGS) $(ADDFLAGS) NetworkHandler.o whatsappClient.cpp -o whatsappClient

NetworkHandler.o: NetworkHandler.cpp
	$(CC) $(CFLAGS) $(ADDFLAGS) -c NetworkHandler.cpp

tar:
	tar cvf ex5.tar $(SOURCES) $(HEADERS) README Makefile Appeal

clean:
	rm -rf whatsappClient whatsappServer *.o *.a *.gch *.tar
