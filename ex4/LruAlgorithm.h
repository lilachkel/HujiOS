//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LRUALGORITHM_H
#define PROJECT_LRUALGORITHM_H

#include "ICacheAlgorithm.hpp"
//#include "LruAlgorithm.cpp"

//template<typename Key, typename Data>
class LruAlgorithm : public ICacheAlgorithm
{
    using Base = ICacheAlgorithm;
    std::list<KeyType> _lru;

    virtual void Update(CacheMap::iterator &cm);
public:
    LruAlgorithm(size_t size);

    ~LruAlgorithm();

    std::pair<KeyType, DataType> FbrGet(KeyType key);

    std::pair<KeyType, DataType> FbrSet(KeyType key, DataType data);

    virtual DataType Get(KeyType key);

    virtual int Set(KeyType key, DataType data);

    virtual void PrintCache(FILE *f);
};


#endif //PROJECT_LRUALGORITHM_H
