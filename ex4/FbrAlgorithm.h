//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_FBRALGORITHM_H
#define PROJECT_FBRALGORITHM_H

#include "ICacheAlgorithm.hpp"
#include "LfuAlgorithm.h"
#include "LruAlgorithm.h"

#define NEW 0
#define MIDDLE 1
#define OLD 2


struct FbrNode
{
    char* _blockBuff;
    int _count;
    int _type;

    FbrNode(char* blockBuff)
    {
        _blockBuff = blockBuff;
        _count = 1;
        _type = NEW;
    }

};

//template<typename Key = std::pair<int, int>, typename Data = void *>
class FbrAlgorithm : public ICacheAlgorithm
{
    using Base = ICacheAlgorithm;
    LruAlgorithm *new_Lru = nullptr;
    LruAlgorithm *m_Lru = nullptr;

    LfuAlgorithm *old_Lfu = nullptr;

    bool m_exist;

    void FreeFbrNode(FbrNode* node)
    {
        free(node->_blockBuff);
        free(node);
    }

    void Update(CacheMap::iterator &cm);

public:

    FbrAlgorithm(size_t size, double f_old, double f_new);

    ~FbrAlgorithm();

    virtual DataType Get(KeyType key);

    virtual int Set(KeyType key, DataType data);

    virtual void PrintCache(FILE *f);

    void SetNew(KeyType key, FbrNode *node);

    void SetM(std::pair<KeyType, FbrNode *>);

    void SetOld(std::pair<KeyType, FbrNode *>);


};


#endif //PROJECT_FBRALGORITHM_H
