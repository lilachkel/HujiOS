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
protected:
    size_t _size;
    std::list<int> _lru;
    CacheMap _cache;

    virtual void Update(CacheMap::iterator &cm) = 0;

public:
    ICacheAlgorithm();

    virtual ~ICacheAlgorithm()
    {
        for (auto &item : _cache)
            delete[] item.second.first;

        _cache.clear();
    }

    virtual char *Get(int key) = 0;

    virtual int Set(int key, char *page) = 0;

};


#endif //PROJECT_ICACHEALGORITHM_H
