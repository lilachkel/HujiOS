#include <stdlib.h>
#include <iostream>

int portNum;

int main(int argc, char **argv)
{
    if (argc != 2 || (portNum = atoi(argv[1])) < 1)
    {
        std::cout << "Usage: whatsappServer portNum" << std::endl;
        exit(EXIT_FAILURE);
    }



    return EXIT_SUCCESS;
}