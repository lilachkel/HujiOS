#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include "NetworkHandler.h"

int main(int argc, char **argv)
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_port = (in_port_t) htons(4200);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        return -1;
    }

    std::string m("hello");
    std::string enc = Encode(m);
    while (SendData(sock, enc) != -1)
    {
        std::cout << ReadData(sock) << std::endl;
    }
    SendData(sock, Encode("exit"));
    close(sock);

    return 0;
}
