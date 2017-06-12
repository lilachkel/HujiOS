#ifndef PROJECT_ICACHEALGORITHM_H
#define PROJECT_ICACHEALGORITHM_H

#include <stdlib.h>
#include <unordered_map>
#include <list>
#include <iostream>
#include <unistd.h>

template<class T, typename U>
struct PairHash
{
    const size_t operator()(const std::pair<T, U> &key) const
    {
        return std::hash<T>()(key.first) ^ std::hash<U>()(key.second);
    }
};

template<class T, typename U>
struct PairEqual
{
    bool operator()(const std::pair<T, U> &lhs, const std::pair<T, U> &rhs) const
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

template<typename Key, typename Data>
using CacheMap = std::unordered_map<Key, std::pair<Data, typename std::list<Key>::iterator>, PairHash<int, int>,
        PairEqual<int, int>>;

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

    ~ICacheAlgorithm()
    {
        CleanCache();
    }

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

    void CleanCache()
    {
        for (auto &item : _cache)
        {
            free(item->second.first);
        }

        _cache.clear();
    }

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

    virtual void PrintCache(FILE *f) = 0;

};


#endif //PROJECT_ICACHEALGORITHM_H
