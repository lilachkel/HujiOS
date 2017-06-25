//
// Created by jenia90 on 6/20/17.
//

#include <iostream>
#include "NetworkHandler.h"


int GetNextMsg(FILE *f, char *buf, size_t bufsize)
{
    if (f == NULL)
        return -1;
    int count = 0;
    int nextChar;
    while (count < bufsize)
    {
        nextChar = getc(f);
        if (nextChar == EOF)
        {
            if (count > 0) return -1;
        }
        if (nextChar == '\n')
        {
            break;
        }
        buf[count++] = nextChar;
    }
    if (nextChar != '\n')
        return -1;
    return count;
}

std::string ReadData(int fd)
{
    char buffer[MAX_MESSAGE_LENGTH];
    FILE *in = fdopen(fd, "r");
    if (in == NULL)
        return "ERROR";
    GetNextMsg(in, buffer, MAX_MESSAGE_LENGTH);

    if (buffer == NULL)
        return "ERROR";

    fclose(in);

    return std::string(buffer);
}

int PutMsg(char *buf, size_t msgSize, FILE *out)
{
    if (fwrite(buf, 1, msgSize, out) != msgSize)
        return -1;
    fflush(out);
    return msgSize;
}

int SendData(int fd, std::string message)
{
    FILE *out = fdopen(fd, "w");
    if (out == NULL)
        return -1;
    char buf[message.length()];
    strcpy(buf, message.c_str());
    return PutMsg(buf, message.length(), out);
//    if (fwrite(message.c_str(), sizeof(message.c_str()), 1, out) != 1)
//    {
//        std::cerr << "ERROR: send(): " << errno << std::endl;
//        return -1;
//    }
//
//    fflush(out);
//    fclose(out);
//    return 0;
//    if (message.length() > MAX_MESSAGE_LENGTH)
//        return -1;
//
//    char buffer[MAX_MESSAGE_LENGTH + 1];
//    strcpy(buffer, message.c_str());
//    int result = send(fd, buffer, sizeof(buffer), 0);
//    return result;
}

std::string Encode(std::string message)
{
    std::stringstream ss;

    ss << message << MESSAGE_END;

    return ss.str();
}

std::tuple<std::string, std::string, std::string> Decode(std::string data)
{

    std::vector<std::string> users;
    std::regex recvReg("(create_group|send|who|exit)(?: ([a-zA-Z]+\\d*) (.*))?");

    std::smatch m;
    std::regex_search(data, m, recvReg);

    return std::make_tuple(m[1].str(), m[2].str(), m[3].str());
}
