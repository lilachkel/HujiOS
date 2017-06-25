#include <stdlib.h>
#include <netinet/in.h>
#include <iostream>
#include "definitions.h"
#include "NetworkHandler.h"
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>


int GetSocket(char *hostname,  unsigned short portnum)
{
    struct sockaddr_in Caddr;
    struct hostent *hp;
    if ((hp= gethostbyname (hostname)) == NULL) {
        exit(EXIT_FAILURE);//todo:error mess
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        std::cout << "ERROR: socket "<< errno << ".\n"<<std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&Caddr, 0, sizeof(Caddr));
//    memcpy((char *)&Caddr.sin_addr , hp->h_addr , (size_t)hp->h_length)//todo: maybe this?
    inet_aton(hp->h_addr,&(Caddr.sin_addr));
//    Caddr.sin_addr.s_addr = inet_aton();
    Caddr.sin_port = htons(portnum);
    Caddr.sin_family = AF_INET;
    if(connect(fd, (struct sockaddr *)&Caddr, sizeof(Caddr))<0)
    {
        close(fd);
        std::cout << "ERROR: connect "<<errno << ".\n"<<std::endl;
        exit(EXIT_FAILURE);
    }
    return fd;
}

void ExpecConnCom(int soket_Cfd , fd_set *tempset,fd_set *readset)
{
    //wait for connect serv mess: CON_SUCCESS \INVALID_USERNAME
    std::string data;
    memcpy(tempset, readset, sizeof(readset));
    int result = select( 1, tempset, NULL, NULL, NULL);//correct?

    // Error case
    if (result < 0) {
        perror("ERROR: select(): ");
        close(soket_Cfd);
        exit(EXIT_FAILURE);
    }
    else if (result == 0) {
        //the server dosent exist- the connect didnt work
        //todo: message??

        close(soket_Cfd);
        exit(EXIT_FAILURE);
    }
    if (FD_ISSET(soket_Cfd, tempset))
    {
        data = ReadData(soket_Cfd);
        std::cout << data << std::endl;
        if(data.compare(INVALID_USERNAME))
        {
            close(soket_Cfd);
            exit(EXIT_FAILURE);
        }
    }
}
int ExpecComunicat()
{
}

int Comunicat(int soket_Cfd, std::string input)
{
    bool ExpectServerConn = true;
    int result;
    fd_set tempset, readset;
    FD_ZERO(&readset);
    FD_ZERO(&tempset);
    FD_SET(soket_Cfd, &readset);
    int maxfd = soket_Cfd;
    SendData(soket_Cfd, input);// problem? since we do it outside of the loop... maybe it wont be fast enough
    ExpecConnCom(soket_Cfd , &tempset, &readset);
    FD_SET(STDIN_FILENO, &readset);
    while(true)
    {
        std::string data;
        memcpy(&tempset, &readset, sizeof(readset));
        result = select(maxfd + 1, &tempset, NULL, NULL, NULL);//maybe just 2? 3?

        // Error case
        if (result < 0) {
            perror("ERROR: select(): ");
            close(soket_Cfd);
            //send exit to server?
            exit(EXIT_FAILURE);
        }
        else if (result == 0) {
            //the server closed the socket
            //todo: message

            close(soket_Cfd);
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &tempset))
        {
            std::string ExpectedData;
            data = ReadData(STDIN_FILENO);
//            switch ()
//            {
//
//            }
            //
// create group data to - CREATE_GROUP_CMD + " OK!"
    //  fail: CREATE_GROUP_CMD + " FAIL!"
//send for1 or for group: SEND_CMD + " OK!"
    //fail: SEND_CMD + " FAIL!"
// exit :"exit OK!"
//WHO_CMD:



            //after sending a message - waits for 'OK'
            std::string data2;
            int result2;
            fd_set tempset2, readset2;
            FD_ZERO(&readset2);
            FD_ZERO(&tempset2);
            FD_SET(soket_Cfd, &readset2);
            result2 = select(maxfd + 1, &tempset2, NULL, NULL, NULL);//maybe maxfd == 1?
            if (result2 < 0) {
                perror("ERROR: select(): ");
                close(soket_Cfd);
                //send exit to server?
                exit(EXIT_FAILURE);
            }
            else if (result2 == 0) {
                //the server closed the socket
                //todo: message

                close(soket_Cfd);
                exit(EXIT_FAILURE);
            }
        }
        if (FD_ISSET(soket_Cfd, &tempset))
        {
            //rec message only... the only case left... no need to check
            data = ReadData(soket_Cfd);
            std::cout << data << std::endl;// print the recv message
        }

    }

}
int main(int argn, char **argv)
{

    // check param serverAddress
    int portNum;
    char *hostName = argv[2];
    if (argn != 4 || (portNum = atoi(argv[3])) < IPPORT_RESERVED || (atoi(hostName)) < 1)
    {
        std::cout << "Usage: whatsappClient clientName serverAddress serverPort" << std::endl;//todo error mess
        exit(EXIT_FAILURE);
    }
    int soket_Cfd = GetSocket(hostName, (unsigned short)portNum);

    return EXIT_SUCCESS;
}