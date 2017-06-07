//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_ICACHEALGORITHM_H
#define PROJECT_ICACHEALGORITHM_H

#include <stdlib.h>
#include <unordered_map>
#include <list>

typedef std::unordered_map<int, std::pair<char *, std::list<int>::iterator>> CacheMap;

class ICacheAlgorithm
{
    void CleanCache(CacheMap &cm)
    {
        for (auto &item : cm)
            delete[] item.second.first;

        cm.clear();
    }

protected:
    size_t _size;
    std::list<int> _queue;
    CacheMap _cache;

    virtual void Update(CacheMap::iterator &cm) = 0;

public:
    ICacheAlgorithm(size_t size) : _size(size)
    {}

    ICacheAlgorithm(ICacheAlgorithm &&other) : _size(std::move(other._size)),
                                               _queue(std::move(other._queue)),
                                               _cache(std::move(other._cache))
    {}

    ICacheAlgorithm &operator=(ICacheAlgorithm &&other)
    {
        if (this != &other)
        {
            _size = other._size;
            _queue = std::move(other._queue);
            _cache = std::move(other._cache);
        }

        return *this;
    }

    virtual ~ICacheAlgorithm()
    {
        CleanCache(_cache);
    }

    virtual char *Get(int key) = 0;
    virtual int Set(int key, char *page) = 0;

};


#endif //PROJECT_ICACHEALGORITHM_H
