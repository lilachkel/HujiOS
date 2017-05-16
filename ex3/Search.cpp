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

#define MULTI_THREAD_LEVEL 10

/**
 * Prints the output vector.
 */
void PrintOutput(OUT_ITEMS_VEC &output)
{
    for (auto &i : output)
        std::cout << i.first << " ";
    std::cout << std::endl;
}

/**
 * Release all resources allocated to the input and output vectors.
 * @tparam T IN_ITEMS_VEC or OUT_ITEMS_VEC.
 * @param v item vector.
 */
template<typename T>
void PointerDestroyer(T &v)
{
    for (auto &p : v)
    {
        delete p.first;
        delete p.second;
    }
    v.clear();
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
    IN_ITEMS_VEC _inputVec;

    for (int i = FIRST_DIR; i < argn; i++)
    {
        if (fs::is_directory(std::string(argv[i])) && !fs::is_empty(std::string(argv[i])))
        {
            _inputVec.push_back({new FileNameKey(std::string(argv[i])), nullptr});
        }
    }

    OUT_ITEMS_VEC _outputVec = RunMapReduceFramework(finder, _inputVec, MULTI_THREAD_LEVEL, true);

    PrintOutput(_outputVec);

    PointerDestroyer(_inputVec);
    PointerDestroyer(_outputVec);

    return EXIT_SUCCESS;
}