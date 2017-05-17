#ifndef PROJECT_SEARCHMAPREDUCE_H
#define PROJECT_SEARCHMAPREDUCE_H

#include "MapReduceFramework.h"
#include "Logger.h"
#include "FileNameKey.cpp"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class SearchMapReduce : public MapReduceBase
{
    std::string _searchStr;
public:
    SearchMapReduce(std::string searchStr) : _searchStr(searchStr)
    {}

    void virtual Map(const k1Base *const key, const v1Base *const val) const;

    void virtual Reduce(const k2Base *const key, const V2_VEC &vals) const;
};

#endif //PROJECT_SEARCHMAPREDUCE_H
