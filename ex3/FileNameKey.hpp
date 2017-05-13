#ifndef PROJECT_FILENAMEKEY_HPP
#define PROJECT_FILENAMEKEY_HPP

#include "MapReduceClient.h"
#include <string>

class FileNameKey : k1Base, k2Base, k3Base, v1Base, v2Base, v3Base
{
    std::string _data;

public:
    FileNameKey() : _data("") {}
    FileNameKey(std::string data) : _data(data) {}
    virtual ~FileNameKey() { _data.clear(); }

    virtual bool operator<(const FileNameKey& other) final
    {
        return std::string(_data) < std::string(other._data);
    }
};

#endif //PROJECT_FILENAMEKEY_HPP
