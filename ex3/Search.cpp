#include "stdlib.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include "MapReduceFramework.h"
#include "FileNameKey.hpp"

#define FIRST_DIR 2
#define MIN_ARGS 3

std::vector<std::string> output;

// TODO: Test if it works with the lambda.
//bool compare(FileNameKey& k1, FileNameKey& k2)
//{
//    return k1 < k2;
//}

class NameMapReduce : MapReduceBase
{
    IN_ITEMS_VEC _inItemsVec;
public:
    virtual void Map(const k1Base *const key, const v1Base *const val) const final
    {
        _inItemsVec.push_back();
    }

    virtual void Reduce(const k2Base *const key, const V2_VEC &vals) const final
    {
    }
};

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
    std::sort(output.begin(), output.end(),
              [&] (FileNameKey& k1, FileNameKey& k2) { return k1 < k2; }
        );
    PrintOutput();

    return EXIT_SUCCESS;
}