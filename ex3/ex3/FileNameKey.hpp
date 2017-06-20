#ifndef PROJECT_FILENAMEKEY_HPP
#define PROJECT_FILENAMEKEY_HPP

#include "MapReduceClient.h"
#include <string>

#define LESS_OP(o) bool operator<(const o &other) const override            \
                    {                                                       \
                        auto o = dynamic_cast<const FileNameKey&>(other);   \
                        return _data < o._data;                             \
                    }

class FileNameKey : public k1Base, public k2Base, public k3Base,
                    public v1Base, public v2Base, public v3Base
{
    std::string _data;

public:
    FileNameKey() : _data("")
    {}

    FileNameKey(std::string data) : _data(data)
    {}

    ~FileNameKey() override
    { _data.clear(); }

    inline std::string GetData() const
    {
        return _data;
    }

    LESS_OP(k1Base)

    LESS_OP(k2Base)

    LESS_OP(k3Base)
};

#endif //PROJECT_FILENAMEKEY_HPP
