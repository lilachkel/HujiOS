//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_FBRALGORITHM_H
#define PROJECT_FBRALGORITHM_H

#include "ICacheAlgorithm.hpp"
template<typename Key = std::pair<int,int>, typename Data = char *>

class FbrAlgorithm : public ICacheAlgorithm<Key, Data>
{
    using Base = ICacheAlgorithm<Key, Data>;
    virtual void Update(typename CacheMap<Key, Data>::iterator &cm);


public:
    FbrAlgorithm(size_t size) : ICacheAlgorithm<Key, Data>(size)
    {}

    ~FbrAlgorithm()
    {
//        delete _head;
    }

    virtual void RemoveByFileID(int fd);

    virtual Data Get(Key key);

    virtual int Set(Key key, Data data);

    virtual void PrintCache(FILE *f);

};


#endif //PROJECT_FBRALGORITHM_H
