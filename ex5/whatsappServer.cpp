#include <stdlib.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <zconf.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "definitions.h"

#define UID std::string
#define GID std::string

std::unordered_map<UID, int> uidToFd;
std::unordered_map<int, UID> fdToUid;
std::unordered_map<UID, GID> uidToGid;
std::unordered_map<GID, fd_set> gidToFdSet;

/**
 * Opens a new socket and starts listening for connections
 * @param port port number to listen on
 * @return new socket fd.
 */
int CreateServer(int port)
{
    struct sockaddr_in addr;
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    if (bind(lfd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
        perror("ERROR: bind(): ");
        close(lfd);
        exit(EXIT_FAILURE);
    }

    listen(lfd, 5);
    return lfd;
}

/**
 * Reads data from a given file descriptor
 * @param fd file descriptor to read from
 * @return string message
 */
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

/**
 * Sends message to the given file descriptor
 * @param fd destination file descriptor
 * @param message string message to send
 * @return -1 in case of an error; non-negative value otherwise.
 */
int SendData(int fd, std::string message)
{
    if (message.length() > MAX_MESSAGE_LENGTH)
        return -1;

    char buffer[MAX_MESSAGE_LENGTH + 1];
    buffer = message.c_str();

    return send(fd, buffer, sizeof(buffer), 0);
}

/**
 * Splits the given data string using a pre-defined delimeter.
 * @param data string to parse
 * @return list of strings
 */
std::list<std::string> ParseData(std::string data)
{
    std::list<std::string> splitData;
    size_t pos = 0;
    std::string token;
    while ((pos = data.find(MESSAGE_DELIMETER)) != std::string::npos)
    {
        token = data.substr(0, pos);
        splitData.push_back(token);
        data.erase(0, pos + 1);
    }
    return splitData;
}

/**
 * Updates the given fd set with all the pending connections.
 * @param set pointer to file descriptor set to update
 * @param servfd listening fd
 * @param maxfd pointer to the current max fd number
 */
int GetNewConnections(fd_set *set, int servfd, int *maxfd)
{
    int new_fd;
    std::string name;
    do
    {
        if (*maxfd < MAX_CLIENTS)
        {
            new_fd = accept(servfd, NULL, NULL);
            name = ReadData(new_fd);
            if (uidToFd.find(name) == uidToFd.end())
            {
                FD_SET(new_fd, set);
                uidToFd[name] = new_fd;
                if (*maxfd < new_fd)
                    *maxfd = new_fd;
            }
            else
            {
                SendData(new_fd, INVALID_USERNAME);
            }
        }
    } while (new_fd != -1);
}

/**
 * Creates a new group with a given name and adds all the requested members to it.
 * @param client file descriptor associated with the client who created the group
 * @param group list of parameters
 * @return -1 in case of failure; 0 otherwise.
 */
int CreateGroup(int client, std::list<std::string> &group)
{
    // In case there are not enough members
    if (group.size() < 3)
        return -1;

    fd_set groupfd;
    FD_ZERO(&groupfd);
    FD_SET(client, &groupfd);
    auto it = group.begin();
    std::advance(it, 1);
    std::string gid = *it;
    std::advance(it, 1);
    for (; it != group.end(); it++)
    {
        auto item = uidToFd.find(*it);
        if (item != uidToFd.end())
        {
            if (FD_ISSET(item->second, &groupfd) == 0)
            {
                FD_SET(item->second, &groupfd);
            }
        }
    }

    gidToFdSet[gid] = groupfd;
    return 0;
}

/**
 * Runs a server on the given port
 * @param portNum integer port number
 * @return 0 if exited cleanly.
 */
int RunServer(int portNum)
{
    int result, desc_ready, servfd = CreateServer(portNum);
    fd_set masterfds, workingfds;
    FD_ZERO(&masterfds);
    FD_ZERO(&workingfds);
    FD_SET(STDIN_FILENO, &masterfds);
    FD_SET(servfd, &masterfds);
    int maxfd = servfd;

    do
    {
        std::string data;
        memcpy(&workingfds, &masterfds, sizeof(masterfds));
        result = select(maxfd + 1, &workingfds, NULL, NULL, NULL);

        // Error case
        if (result < 0)
        {
            perror("ERROR: select(): ");
            break;
        }
            // Timeout case
        else if (result == 0)
        {
            continue;
        }

        desc_ready = result;
        for (int i = 0; i < maxfd && desc_ready > 0; ++i)
        {
            if (FD_ISSET(i, &workingfds))
            {
                desc_ready--;
                // if listening fd then get all pending connections.
                if (i == servfd)
                {
                    GetNewConnections(&masterfds, servfd, &maxfd);
                }
                else if (i == 0)
                {
                    if (ReadData(i) == "EXIT")
                    {
                        // TODO: close all connections and exit.
                    }
                }
                else
                {
                    data = ReadData(i);
                    std::list<std::string> command = ParseData(data);
                    if (command.front() == CREATE_GROUP_CMD)
                    {
                        std::advance(command, 1);
                        if (CreateGroup(i, command) == -1)
                            std::cerr << CREATE_GRP_FAILURE(fdToUid[i], *std::next(command.begin())) << std::endl;
                    }
                }
            }
        }
    } while (true);

    close(servfd);
    return 0;
}

int main(int argc, char **argv)
{
    int portNum;
    if (argc != 2 || (portNum = atoi(argv[1])) < IPPORT_RESERVED)
    {
        std::cout << "Usage: whatsappServer portNum" << std::endl;
        exit(EXIT_FAILURE);
    }

    return RunServer(portNum);
}