//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LRUALGORITHM_H
#define PROJECT_LRUALGORITHM_H

#include "ICacheAlgorithm.hpp"

class LruAlgorithm : public ICacheAlgorithm
{
    virtual void Update(CacheMap::iterator &cm);
public:
    LruAlgorithm(size_t size) : _size(size)
    {}

    virtual ~LruAlgorithm();

    virtual char *Get(int key);

    virtual int Set(int key, char *page);
};


#endif //PROJECT_LRUALGORITHM_H
