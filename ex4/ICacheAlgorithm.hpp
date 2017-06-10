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

    /**
     * Updates the cache buffer.
     * @param cm position iterator for the cache queue
     */
    virtual void Update(typename CacheMap<Key, Data>::iterator &cm) = 0;

public:
    ICacheAlgorithm(size_t size) : _capacity(size)
    {}

    ICacheAlgorithm(ICacheAlgorithm &&other) : _capacity(std::move(other._capacity)),
                                               _cache(std::move(other._cache))
    {}

    /**
     * Assignment move operator
     * @param other the object to move from
     * @return updated object reference
     */
    ICacheAlgorithm<Key, Data> &operator=(ICacheAlgorithm<Key, Data> &&other)
    {
        if (this != &other)
        {
            _capacity = other._capacity;
            _cache = std::move(other._cache);
        }

        return *this;
    }

    /**
     * Dtor
     */
    virtual ~ICacheAlgorithm() = 0;

    /**
     * Gets an item from the cache by a given key
     * @param key item key
     * @return Data item
     */
    virtual Data Get(Key key) = 0;

    /**
     * Adds an item to the cache (buffer)
     * @param key item key to be added to the cache
     * @param page item data to be added
     * @return 0 if all goes well; -1 if error.
     */
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
