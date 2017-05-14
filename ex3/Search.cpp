#include "stdlib.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <dirent.h>
#include "MapReduceFramework.h"
#include "FileNameKey.hpp"
#include "Logger.h"
#include <experimental/filesystem>

#define SEARCH_STR 1
#define FIRST_DIR 2
#define MIN_ARGS 3

std::vector<std::string> output;

namespace stdfs = std::experimental::filesystem;

// TODO: Test if it works with the lambda.
//bool compare(FileNameKey& k1, FileNameKey& k2)
//{
//    return k1 < k2;
//}

class SearchMapReduce : MapReduceBase
{
    std::string _searchStr;
    Logger _logger;
public:
    SearchMapReduce(std::string searchStr, Logger &logger) : _searchStr(searchStr), _logger(logger)
    {}

    void Map(const k1Base *const key, const v1Base *const val) const override
    {
        auto _key = dynamic_cast<const FileNameKey *const>(key);
        auto _dir = _key->GetData();
        auto path = stdfs::path(_dir);
        const stdfs::directory_iterator end{} ;

        for( stdfs::directory_iterator iter{path} ; iter != end ; ++iter )
        {
            auto file = iter->path().filename().string();
            if (file.find(_searchStr) != std::string::npos)
                Emit2(new FileNameKey(iter->path()), new FileNameKey(file));
        }
// TODO: Test if this  experimental shit works.
//        DIR* dir;
//        struct dirent* ent;
//        if((dir = opendir(_dir.c_str())) != NULL)
//        {
//            while((ent = readdir(dir)) != NULL)
//            {
//
//            }
//            closedir(dir);
//        }
//        else
//        {
//            _logger.Log("invalid directory");
//        }
    }

    void Reduce(const k2Base *const key, const V2_VEC &vals) const override
    {
        auto _key = dynamic_cast<const FileNameKey *const>(key);
        // TODO: Continue working here
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
    Logger logger = Logger(".MapReduceFrameworkLog");

    auto finder = SearchMapReduce(argv[SEARCH_STR], logger);
    for(int i = FIRST_DIR; i < argn; i++)
    {
        k1Base *searchString = new FileNameKey(argv[i]);

        finder.Map(searchString, nullptr);

        delete searchString;
    }

    // Sort the output vector.
    std::sort(output.begin(), output.end(),
              [&] (FileNameKey& k1, FileNameKey& k2) { return k1 < k2; }
        );
    PrintOutput();

    return EXIT_SUCCESS;
}