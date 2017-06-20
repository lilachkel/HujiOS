//
// Created by jenia90 on 6/20/17.
//

#include <sstream>
#include <tuple>
#include "NetworkHandler.h"


std::string ReadData(int fd)
{
    char buffer[MAX_MESSAGE_LENGTH + 1];
    int result = recv(fd, buffer, sizeof(buffer), 0);
    if (result < 0)
    {
        return "ERROR";
    }
    else if (result == 0)
    {
        return EXIT_CMD;
    }

    return std::string(buffer);
}

int SendData(int fd, std::string message)
{
    if (message.length() > MAX_MESSAGE_LENGTH)
        return -1;

    char buffer[MAX_MESSAGE_LENGTH + 1];
    buffer = message.c_str();

    return send(fd, buffer, sizeof(buffer), 0);
}

std::string readyForSend(std::string message)
{
    std::stringstream ss;

    ss << MESSAGE_START << message.length() << MESSAGE_LENGTH_END << message << MESSAGE_END;

    return ss.str();
}

std::tuple<std::string, std::string, std::string> ParseData(std::string data)
{
    std::tuple<std::string, std::string, std::string> splitData;
    size_t pos;
    std::string token;
    pos = data.find(MESSAGE_LENGTH_END);


    return splitData;
}
