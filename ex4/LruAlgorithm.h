//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LRUALGORITHM_H
#define PROJECT_LRUALGORITHM_H

#include "ICacheAlgorithm.hpp"

class LruAlgorithm : public ICacheAlgorithm
{

public:
    LruAlgorithm(size_t blkSize, size_t blkNum);

    virtual ~LruAlgorithm();
};


#endif //PROJECT_LRUALGORITHM_H
