//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LFUALGORITHM_H
#define PROJECT_LFUALGORITHM_H

#include "ICacheAlgorithm.hpp"

class LfuAlgorithm : public ICacheAlgorithm
{
public:
    LfuAlgorithm(size_t blkSize, size_t blkNum);

    virtual ~LfuAlgorithm();
};


#endif //PROJECT_LFUALGORITHM_H
