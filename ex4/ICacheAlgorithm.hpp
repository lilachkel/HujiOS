//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_ICACHEALGORITHM_H
#define PROJECT_ICACHEALGORITHM_H

#include <stdlib.h>

class ICacheAlgorithm
{
    size_t _blksize, _blocks_num;
public:
    ICacheAlgorithm() = delete;

    virtual ~ICacheAlgorithm() = 0;
};


#endif //PROJECT_ICACHEALGORITHM_H
