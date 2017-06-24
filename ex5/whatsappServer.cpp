#include <stdlib.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <unistd.h>
#include <regex>
#include <sys/socket.h>
#include <netinet/in.h>
#include "definitions.h"
#include "NetworkHandler.h"

enum IdType
{
    GROUP,
    USER
};

struct Comparator
{
    bool operator()(const std::string lhs, const std::string rhs)
    {
        return lhs < rhs;
    }
};

#define MAX_PENDING 10
#define UID std::string
#define GID std::string

std::map<UID, int, Comparator> uidToFd;
std::unordered_map<int, UID> fdToUid;
std::unordered_map<UID, GID> uidToGid;
std::unordered_map<UID, IdType> uidToType;
std::unordered_map<GID, fd_set> gidToFdSet;

/**
 * Splits a given string into a vector
 * @param what the string to split
 * @param delimeter delimeter to split by
 * @return vector of strings
 */
std::vector<std::string> SplitString(std::string what, char delimeter = ',')
{
    std::vector<std::string> result;
    size_t pos;
    while ((pos = what.find(delimeter)) != std::string::npos)
    {
        result.push_back(what.substr(0, pos));
        what.erase(0, pos + 1);
    }

    return result;
}

/**
 * Opens a new socket and starts listening for connections
 * @param port port number to listen on
 * @return new socket fd.
 */
int CreateServer(int port)
{
    int lfd;
    if ((lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        std::cerr << "ERROR: socket() " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_port = (in_port_t) htons(port);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(lfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        std::cerr << "ERROR: bind(): " << errno << std::endl;
        close(lfd);
        exit(EXIT_FAILURE);
    }

    if (listen(lfd, MAX_PENDING) < 0)
    {
        std::cerr << "ERROR: listen(): " << errno << std::endl;
        close(lfd);
        exit(EXIT_FAILURE);
    }
    return lfd;
}

/**
 * Closes all of the open connections
 * @param servfd server file descriptor
 */
void ShutdownServer(int servfd)
{
    for (auto &c : uidToFd)
        close(c.second);

    close(servfd);
}

/**
 * Updates the given fd set with all the pending connections.
 * @param set pointer to file descriptor set to update
 * @param servfd listening fd
 * @param maxfd pointer to the current max fd number
 */
void AcceptConnections(fd_set *set, int servfd, int *maxfd)
{
    int new_fd;
    std::string name;
    struct sockaddr_storage remoteHost;
    socklen_t addrlen = sizeof(remoteHost);

    do
    {
        // proceed only if we can still add more clients.
        if (*maxfd < MAX_CLIENTS)
        {
            if ((new_fd = accept(servfd, (struct sockaddr *) &remoteHost, &addrlen) != -1))
            {
                name = ReadData(new_fd);
                if (name.compare("ERROR") != 0)
                    if (uidToFd.find(name) == uidToFd.end())
                    {
                        FD_SET(new_fd, set);
                        uidToFd[name] = new_fd;
                        uidToType[name] = USER;
                        SendData(new_fd, Encode(CON_SUCCESS));
                        if (*maxfd < new_fd)
                            *maxfd = new_fd;
                    }
                    else
                    {
                        SendData(new_fd, Encode(INVALID_USERNAME));
                    }
            }
        }
    } while (new_fd != -1);
}

/**
 * Creates a new group with a given name and adds all the requested members to it.
 * @param group pair of group name and its fd
 * @param users string containing list of users split by a single comma.
 * @return -1 in case of failure; 0 otherwise.
 */
int CreateGroup(std::pair<std::string, int> group, std::string users)
{
    auto userList = SplitString(users, ',');
    if (userList.size() <= 1)
        return -1;

    fd_set groupfd;
    FD_ZERO(&groupfd);
    FD_SET(group.second, &groupfd);
    for (auto &u : userList)
    {
        auto item = uidToFd.find(u);
        if (item != uidToFd.end())
        {
            if (FD_ISSET(item->second, &groupfd) == 0)
            {
                FD_SET(item->second, &groupfd);
            }
        }
    }

    gidToFdSet[group.first] = groupfd;
    uidToType[group.first] = GROUP;
    return 0;
}

void UserLogout(int user, int *maxfd, fd_set *fdSet)
{
    std::string username = fdToUid[user];
    auto grp = uidToGid.find(username);
    if (grp != uidToGid.end())
    {
        FD_CLR(user, &gidToFdSet[grp->second]);
    }

    FD_CLR(user, fdSet);
    fdToUid.erase(user);
    if (user == *maxfd)
        (*maxfd)--;
    std::cout << EXIT_REQUEST(username) << std::endl;

}

/**
 * Executes the given command
 * @param maxfd maximal fd count
 * @param src source file descriptor
 * @param cmd command string
 * @param name destination/group name
 * @param args list of usernames or message
 */
void ExecuteCommand(int maxfd, int src, std::string cmd, std::string name, std::string args)
{
    int result;
    if (cmd.compare(CREATE_GROUP_CMD) == 0)
    {
        auto item = uidToFd.find(name);
        if (item != uidToFd.end())
        {
            if (CreateGroup({item->first, item->second}, args) == 0)
            {
                std::cout << CREATE_GRP_SUCCESS(fdToUid[src], item->first) << std::endl;
            }
            else
            {
                std::cout << CREATE_GRP_FAILURE(fdToUid[src], item->first) << std::endl;
            }
        }
        return;
    }
    else if (cmd.compare(SEND_CMD) == 0)
    {
        auto type = uidToType.find(name);
        if (type != uidToType.end())
        {
            std::string message = Encode(fdToUid[src] + ": " + args);
            switch (type->second)
            {
                case USER:
                    result = SendData(uidToFd[name], message);
                    break;
                case GROUP:
                    for (int i = 0; i <= maxfd; i++)
                    {
                        if (FD_ISSET(i, &gidToFdSet[name]) && i != src)
                        {
                            if ((result = SendData(i, message)) != 0)
                            {
                                std::cout << SEND_FAILURE(name, message, fdToUid[i]);
                            }
                        }
                    }
                    break;
            }
        }
    }
    else if (cmd.compare(WHO_CMD) == 0)
    {
        name = fdToUid[src];
        std::cout << WHO_REQUEST(name);
        std::stringstream ss;
        for (auto &uid : uidToFd)
        {
            ss << uid.first << ',';
        }
        args = ss.str();
        args.pop_back();

        result = SendData(src, args);
    }

    //TODO: printing it here might be problematic. Check during debug session.
    if (result == 0)
        std::cout << SEND_SUCCESS(fdToUid[src], args, name) << std::endl;
    else
        std::cout << SEND_FAILURE(fdToUid[src], args, name) << std::endl;
}

/**
 * Runs a server on the given port
 * @param portNum integer port number
 * @return 0 if exited cleanly.
 */
int RunServer(int portNum)
{
    int result, servfd = CreateServer(portNum);
    fd_set masterfds, workingfds;
    FD_ZERO(&masterfds);
    FD_ZERO(&workingfds);
    FD_SET(STDIN_FILENO, &masterfds);
    FD_SET(servfd, &masterfds);
    int mfd, maxfd = servfd;

    do
    {
        std::string data;
        mfd = maxfd;
        memcpy(&workingfds, &masterfds, sizeof(masterfds));
        result = select(mfd + 1, &workingfds, NULL, NULL, NULL);

        // Error case
        if (result < 0)
        {
            perror("ERROR: select(): ");
            ShutdownServer(servfd);
            return EXIT_FAILURE;
        }
            // Timeout case
        else if (result == 0)
        {
            continue;
        }

        for (int i = 0; i < maxfd; ++i)
        {
            if (FD_ISSET(i, &workingfds))
            {
                if (i == STDIN_FILENO)
                {
                    if (ReadData(i).compare("EXIT") == 0)
                    {
                        ShutdownServer(servfd);
                        return EXIT_SUCCESS;
                    }
                }
                else
                {
                    data = ReadData(i);
                    std::string command, name, args;

                    std::tie(command, name, args) = Decode(data);
                    if (command.compare(EXIT_CMD) == 0)
                        UserLogout(i, &maxfd, &masterfds);
                    else
                        ExecuteCommand(maxfd, i, command, name, args);
                }
            }
            AcceptConnections(&masterfds, servfd, &maxfd);
        }
    } while (true);
}

int main(int argc, char **argv)
{
    int portNum;
    if (argc != 2 || (portNum = atoi(argv[1])) < IPPORT_RESERVED)
    {
        std::cout << WRONG_SERVER_USAGE << std::endl;
        exit(EXIT_FAILURE);
    }

    return RunServer(portNum);
}