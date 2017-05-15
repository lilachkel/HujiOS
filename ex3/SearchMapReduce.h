#ifndef PROJECT_SEARCHMAPREDUCE_H
#define PROJECT_SEARCHMAPREDUCE_H

#include "MapReduceFramework.h"
#include "Logger.h"
#include "FileNameKey.hpp"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class SearchMapReduce : MapReduceBase
{
    std::string _searchStr;
    Logger _logger;
public:
    SearchMapReduce(std::string searchStr, Logger &&logger) : _searchStr(searchStr), _logger(std::move(logger))
    {}

    void virtual Map(const k1Base *const key, const v1Base *const val) const;

    void virtual Reduce(const k2Base *const key, const V2_VEC &vals) const;
};

#endif //PROJECT_SEARCHMAPREDUCE_H
