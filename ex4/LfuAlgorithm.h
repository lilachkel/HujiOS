//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LFUALGORITHM_H
#define PROJECT_LFUALGORITHM_H

#include "ICacheAlgorithm.hpp"

struct LfuKey
{
    int key, count = 0;
};

class LfuAlgorithm : public ICacheAlgorithm<int, LfuKey>
{
public:
    LfuAlgorithm(size_t size);
};


#endif //PROJECT_LFUALGORITHM_H
