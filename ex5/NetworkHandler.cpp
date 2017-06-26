#include "NetworkHandler.h"


/**
 * Gets a message from the given file stream and write it to the given buffer.
 * @param in FILE pointer from which we want to read
 * @param buf char array buffer that is going to be populated with the received message
 * @param bufsize buffer size
 * @return number of chars read.
 */
int GetNextMsg(FILE *in, char *buf, size_t bufsize)
{
    if (in == NULL)
        return -1;

    size_t count = 0;
    int nextChar;
    while (count < bufsize)
    {
        nextChar = getc(in);
        if (nextChar == EOF)
        {
            if (count > 0) return -1;
        }
        if (nextChar == MESSAGE_END)
        {
            break;
        }
        buf[count++] = nextChar;
    }
    if (nextChar != MESSAGE_END)
        return -1;

    return count;
}

std::string ReadData(int fd)
{
    char buffer[MAX_MESSAGE_LENGTH], ret[MAX_MESSAGE_LENGTH];
    int count;
    FILE *in = fdopen(fd, "r");
    if (in == NULL)
        return "ERROR";
    if ((count = GetNextMsg(in, buffer, MAX_MESSAGE_LENGTH)) == -1)
        return "ERROR";

    if (buffer == NULL)
        return "ERROR";

    strncpy(ret, buffer, count);
    ret[count] = '\0';

    return std::string(ret);
}

/**
 * Writes the data from the buffer to the output stremm (FILE pointer)
 * @param buf char array to be sent
 * @param msgSize length of message
 * @param out output file pointer
 * @return number of chars sent.
 */
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

    size_t msgSize = message.length();
    char buf[MAX_MESSAGE_LENGTH];
    memset(buf, 0, MAX_MESSAGE_LENGTH);
    strcpy(buf, message.c_str());

    return PutMsg(buf, msgSize, out);
}

std::string Encode(std::string message)
{
    std::stringstream ss;

    ss << message << MESSAGE_END;

    return ss.str();
}

std::tuple<std::string, std::string, std::string> Decode(std::string data)
{

//    std::vector<std::string> users;
    std::regex recvReg("(create_group|send|who|exit)(?: ([a-zA-Z]+\\d*) (.*))?");

    std::smatch m;
    std::regex_search(data, m, recvReg);

    return std::make_tuple(m[1].str(), m[2].str(), m[3].str());
}
