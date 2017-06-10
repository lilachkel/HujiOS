//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_ICACHEALGORITHM_H
#define PROJECT_ICACHEALGORITHM_H

#include <stdlib.h>
#include <unordered_map>
#include <list>
#include <iostream>

template<typename Key, typename Data>
using CacheMap = std::unordered_map<Key, std::pair<Data, typename std::list<Key>::iterator>>;

template<typename Key, typename Data>
class ICacheAlgorithm
{
protected:
    size_t _capacity;
    CacheMap<Key, Data> _cache;

    virtual void Update(typename CacheMap<Key, Data>::iterator &cm) = 0;

public:
    ICacheAlgorithm(size_t size) : _capacity(size)
    {}

    ICacheAlgorithm(ICacheAlgorithm &&other) : _capacity(std::move(other._capacity)),
                                               _cache(std::move(other._cache))
    {}

    ICacheAlgorithm<Key, Data> &operator=(ICacheAlgorithm<Key, Data> &&other)
    {
        if (this != &other)
        {
            _capacity = other._capacity;
            _cache = std::move(other._cache);
        }

        return *this;
    }

    virtual ~ICacheAlgorithm() = 0;
    virtual Data Get(Key key) = 0;
    virtual int Set(Key key, Data page) = 0;

    void PrintCache()
    {
        for (const auto &item : _cache)
        {
            //TODO: how do we want to print it?
        }
    }
};


#endif //PROJECT_ICACHEALGORITHM_H
