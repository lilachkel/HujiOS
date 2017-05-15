#include "stdlib.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include "MapReduceFramework.h"
#include "FileNameKey.hpp"
#include "Logger.h"
#include "SearchMapReduce.h"

#define SEARCH_STR 1
#define FIRST_DIR 2
#define MIN_ARGS 3

std::vector<std::string> _output;

/**
 * Prints the output vector.
 */
void PrintOutput()
{
    for (auto i : _output)
        std::cout << i << " ";
    std::cout << std::endl;
}

int main(int argn, char **argv)
{

        if (argn < MIN_ARGS)
    {
        std::cerr << "Usage: <substring to search> <folder, separated by space>" << std::endl;
        return EXIT_FAILURE;
    }

    // Get the logger and finder ready
    Logger logger = Logger(".MapReduceFrameworkLog", true);
    SearchMapReduce finder = SearchMapReduce(argv[SEARCH_STR], std::move(logger));

    try
    {
        for (int i = FIRST_DIR; i < argn; i++)
        {
            if (fs::is_directory(std::string(argv[i])) && !fs::is_empty(std::string(argv[i])))
            {
                k1Base *dir = new FileNameKey(argv[i]);
                finder.Map(dir, nullptr);
                delete dir;
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    PrintOutput();

    return EXIT_SUCCESS;
}