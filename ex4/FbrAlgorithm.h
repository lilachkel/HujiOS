//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_FBRALGORITHM_H
#define PROJECT_FBRALGORITHM_H

#include <set>
#include "ICacheAlgorithm.hpp"
#include "LruAlgorithm.h"
//#include <list>

#define NEW 0
#define MIDDLE 1
#define OLD 2

struct FbrNode
{
    void *_blockBuff;
    int _count;
    int _type;

    FbrNode(void *blockBuff) : _blockBuff(blockBuff), _count(1), _type(NEW)
    {}

};
struct Comp
{
    const bool operator()(const std::pair<KeyType, size_t> &k1, const std::pair<KeyType, size_t> &k2)
    {
        return k1.second < k2.second;
    }
};
struct Lfu {
    size_t _capacity;
    std::unordered_map<KeyType, FbrNode *, PairHash, PairEqual> Cache;
    std::set<std::pair<KeyType, size_t>, Comp> lfu;
    Lfu(size_t capacity)
    {
        _capacity = capacity;
    }
};

class FbrAlgorithm : public ICacheAlgorithm
{
    using Base = ICacheAlgorithm;
    LruAlgorithm *new_Lru = nullptr;
    LruAlgorithm *m_Lru = nullptr;

    Lfu *old_Lfu = nullptr;

    bool m_exist;

    static void FreeFbrNode(DataType data)
    {
        FbrNode *node = static_cast<FbrNode *>(data);
        if (!node)
            return;
        free(node->_blockBuff);
        free(node);
    }

    void Update(CacheMap::iterator &cm)
    {}

public:

    FbrAlgorithm(size_t size, double f_old, double f_new);

    ~FbrAlgorithm();

    virtual DataType Get(KeyType key);

    virtual int Set(KeyType key, DataType data);

    virtual void PrintCache();

    std::pair<KeyType, FbrNode*> GetOld(KeyType key);


    void SetNew(KeyType key, FbrNode *node);

    void SetM(KeyType key, FbrNode *node);

    void SetOld(KeyType key, FbrNode *node);


};


#endif //PROJECT_FBRALGORITHM_H
