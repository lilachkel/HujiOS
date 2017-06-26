#include <stdlib.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "definitions.h"
#include "NetworkHandler.h"


int GetSocket(char *serverAddress, unsigned short portnum)
{
    struct sockaddr_in Caddr;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        SYS_CALL_FAIL("socket");
        exit(EXIT_FAILURE);
    }

    memset(&Caddr, 0, sizeof(Caddr));
    in_addr_t inaddr = inet_addr(serverAddress);
    Caddr.sin_addr.s_addr = inaddr;
    Caddr.sin_port = htons(portnum);
    Caddr.sin_family = AF_INET;
    if (connect(fd, (struct sockaddr *) &Caddr, sizeof(Caddr)) < 0)
    {
        close(fd);
        SYS_CALL_FAIL("connect");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int sendRequest(int fd, std::string message)
{
    message = Encode(message);
    return SendData(fd, message);
}

void ExpecConnCom(int soket_Cfd, fd_set *tempset, fd_set *readset)
{
    //wait for connect serv mess: CON_SUCCESS \INVALID_USERNAME
    std::string data;
    memcpy(tempset, readset, sizeof(*readset));
    int result = select(FD_SETSIZE, tempset, NULL, NULL, NULL);//correct?

    // Error case
    if (result < 0)
    {
        SYS_CALL_FAIL("select");
        close(soket_Cfd);
        exit(EXIT_FAILURE);
    }
    // case result == 0 deleted
    if (FD_ISSET(soket_Cfd, tempset))
    {
        data = ReadData(soket_Cfd);
        if (data.compare(CON_SUCCESS) == 0)
        {
            std::cout << "Connected successfully." << std::endl;
        }
        else // i assumes the server message is valid - data == INVALID_USERNAME
        {
            std::cout << "Client name is already in use." << std::endl;
            close(soket_Cfd);// correct?
            exit(EXIT_FAILURE);
        }
    }
}

bool ExpecAnAnswer(int soket_Cfd, std::vector<std::string> *serverMessToPrint, std::string ExpectedData)
{
    std::string data2;
    int result2;
    fd_set readset2;
    FD_ZERO(&readset2);
    FD_SET(soket_Cfd, &readset2);
    result2 = select(FD_SETSIZE, &readset2, NULL, NULL, NULL);//maybe maxfd == 1?
    if (result2 < 0)
    {
        SYS_CALL_FAIL("select");
        close(soket_Cfd);
        //send exit to server?
        exit(EXIT_FAILURE);
    }
    else if (result2 == 0)
    {
        //the server closed the socket
        std::cout << "The server is off. Exiting..." << std::endl; // ok?
        close(soket_Cfd);
        exit(EXIT_FAILURE);
    }
    else
    {
        data2 = ReadData(soket_Cfd);
        std::string commandType;
        std::size_t firstPos;
        std::string WithoutCommandType;
        firstPos = data2.find(" ");
        commandType = data2.substr(0, firstPos);
        WithoutCommandType = data2.substr(firstPos + 1, data2.size());
        // SEND "send"  WHO "who" EXIT "exit"
        if (commandType.compare(ExpectedData) == 0)
        {
            std::cout << WithoutCommandType << std::endl;
            if (commandType.compare(EXIT_CMD) == 0)
            {
                exit(0);
            }
        }
        else
        {
            serverMessToPrint->push_back(WithoutCommandType);
            return false;
        }
    }
    return true;
}

int commandValidation(std::string comm)
{
//    std::regex recvReg("(create_group|send|who|exit)");

    if (comm.compare(CREATE_GROUP_CMD) == 0)
    {
        return 1;
    }
    else if (comm.compare(SEND_CMD) == 0)
    {
        return 2;
    }
    else if (comm.compare(WHO_CMD) == 0)
    {
        return 3;
    }
    else if (comm.compare(EXIT_CMD) == 0)
    {
        return 4;
    }
    return -1;
    // CREATE_GROUP_COMM 1 SEND_COMM 2  WHO_COMM 3  EXIT_COMM 4
}

int isNameValid(std::string name)
{
    std::regex nameReg("([a-zA-Z]+\\d*)");

    std::smatch m;

    std::regex_match(name, m, nameReg);

    return std::regex_match(name, m, nameReg);;
}

/*
 *
 * @param data
 * @return :
 * return the command type specify by:
 * CREATE_GROUP_COMM 1 SEND_COMM 2  WHO_COMM 3  EXIT_COMM 4
 * in case of invalid command returns -1
 */
int RequestValidation(std::string data)
{
    std::istringstream buf(data);
    std::istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> tokens(beg, end);
    int commandVal = commandValidation(tokens.front());
    if (commandVal == 1)
    {
        if (tokens.size() < 3)
            return -1;
        if (!isNameValid(tokens[1]))//<group_name
            return 11;
        //tokens[2] contains <list_of_client_names>
        std::replace(tokens[2].begin(), tokens[2].end(), ',', ' '); // replace all ',' to ' '
        std::istringstream buf2(tokens[2]);
        std::istream_iterator<std::string> beg2(buf2), end2;
        std::vector<std::string> names2(beg2, end2);
        for (std::string name : names2)
        {
            if (!isNameValid(name))//<group_name
                return 12;
        }
        return 1;
    }
    else if (commandVal == 2)
    {
        if (tokens.size() < 3)
            return -1;
        if (!isNameValid(tokens[1]))// a client name
            return 21;
        return 2;
    }
    else if (commandVal == 3)
    {
        return 3;
    }
    else if (commandVal == 4)
    {
        return 4;
    }
    return -1;
}

int Comunicat(int soket_Cfd, std::string name)
{
    int result;
    fd_set tempset, readset;
    FD_ZERO(&readset);
    FD_ZERO(&tempset);
    FD_SET(soket_Cfd, &readset);
    sendRequest(soket_Cfd, name);// problem? since we do it outside of the loop... maybe it wont be fast enough
    ExpecConnCom(soket_Cfd, &tempset, &readset);
    FD_SET(STDIN_FILENO, &readset);

    while (true)
    {
        std::vector<std::string> serverMessToPrint;
        std::string data;
        memcpy(&tempset, &readset, sizeof(readset));
        result = select(FD_SETSIZE, &tempset, NULL, NULL, NULL);//maybe just 2? 3?

        // Error case
        if (result < 0)
        {
            SYS_CALL_FAIL("select");
            close(soket_Cfd);
            //send exit to server?
            exit(EXIT_FAILURE);
        }
        else if (result == 0)
        {
            //the server closed the socket
            std::cout << "The server is off. Exiting..." << std::endl; // ok?
            close(soket_Cfd);
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &tempset))
        {

            data = ReadData(STDIN_FILENO);
            int commType = RequestValidation(data); //  commType == 11: invalid group name
            //  commType == 12: invalid users name
            //checks the comm type:
            // CREATE_GROUP_COMM 1 SEND_COMM 2  WHO_COMM 3  EXIT_COMM 4
            std::string ExpectedData;
            switch (commType)
            {
                case 1: //CREATE_GROUP_COMM 1
                    ExpectedData = CREATE_GROUP_CMD;

                    break;
                case 11://  commType == 11: invalid group name
                    std::cout << "ERROR: Invalid group name. A group name can only include letters and digits."
                              << std::endl;
                    continue;
                case 2: //SEND_COMM 2
                    ExpectedData = SEND_CMD;
                    break;
                case 12://  commType == 12: invalid users name
                    std::cout << "ERROR: Invalid client names. A client name can only include letters and digits."
                              << std::endl;
                    continue;
                case 3://WHO_COMM 3
                    ExpectedData = WHO_CMD;
                    break;
                case 4://EXIT_COMM 4
                    ExpectedData = EXIT_CMD;
                    break;
                case 21:
                    std::cout << "ERROR: Invalid client name. A client name can only include letters and digits."
                              << std::endl;
                    continue;
                default: // < 0
                    std::cout << INVALID_CMD << std::endl;
                    continue;
                    // all cases for easy debug...
            }

            sendRequest(soket_Cfd, data);
            //after sending a message - waits for 'OK'
            bool gotAnswer;
            do
            {
                gotAnswer = ExpecAnAnswer(soket_Cfd, &serverMessToPrint, ExpectedData);
            } while (!gotAnswer);
        }
        if (FD_ISSET(soket_Cfd, &tempset))
        {
            //rec message only... the only case left... no need to check
            data = ReadData(soket_Cfd);
            std::cout << data << std::endl;// print the recv message
        }

    }
    return 0;
}

int main(int argn, char **argv)
{

    // check param serverAddress
    int portNum;
    char *hostName = argv[2];
    char *userName = argv[1];
    if (argn != 4 || (portNum = atoi(argv[3])) < IPPORT_RESERVED || isNameValid(userName) < 0)
    {
        std::cout << WRONG_CLIENT_USAGE << std::endl;
        exit(EXIT_FAILURE);
    }

    int soket_Cfd = GetSocket(hostName, (unsigned short) portNum);
    return Comunicat(soket_Cfd, std::string(userName));
}