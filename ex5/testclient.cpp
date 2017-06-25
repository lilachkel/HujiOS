
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "NetworkHandler.h"

int main(int argc, char **argv)
{
    int sock, result;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_port = (in_port_t) htons(4200);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    fd_set fdSet;
    FD_ZERO(&fdSet);
    FD_SET(sock, &fdSet);
    FD_SET(STDIN_FILENO, &fdSet);
    bool connected = false;

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        return -1;
    }
    SendData(sock, Encode("user1"));
    std::cout << ReadData(sock) << std::endl;
    do
    {
//        connected = true;
        std::string message;
        result = select(sock + 1, &fdSet, NULL, NULL, NULL);
        message = ReadData(STDIN_FILENO);
        SendData(sock, Encode(message));
        std::cout << ReadData(sock) << std::endl;
        std::cout << ReadData(sock) << std::endl;
    } while (connected && result != -1);

    close(sock);

    return 0;
}
