//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LRUALGORITHM_H
#define PROJECT_LRUALGORITHM_H

#include "ICacheAlgorithm.hpp"

template<typename Key, typename Data>
class LruAlgorithm : public ICacheAlgorithm<Key, Data>
{
    using Base = ICacheAlgorithm<Key, Data>;
    std::list<Key> _lru;

    virtual void Update(typename CacheMap<Key, Data>::iterator &cm);
public:
    LruAlgorithm(size_t size);

    ~LruAlgorithm();

    virtual Data Get(Key key);

    virtual int Set(Key key, Data page);

    virtual void PrintCache(FILE *f);
};


#endif //PROJECT_LRUALGORITHM_H
