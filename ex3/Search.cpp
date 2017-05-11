#include "stdlib.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include "MapReduceFramework.h"

#define FIRST_DIR 2
#define MIN_ARGS 3

std::vector<std::string> output;



void PrintOutput()
{
    for(auto i : output)
        std::cout << i;
    std::cout << std::endl;
}

int main(int argn, char ** argv)
{
    std::fstream currFile;
    if(argn < MIN_ARGS)
    {
        return EXIT_FAILURE;
    }

    for(int i = FIRST_DIR; i < argn; i++)
    {
        currFile.open(argv[i]); // Open i-th path
        if(!currFile.is_open()) // Check if path is correct
            return EXIT_FAILURE;



        currFile.close();
    }

    // Sort the output vector.
    std::sort(output.begin(), output.end());
    PrintOutput();

    return EXIT_SUCCESS;
}