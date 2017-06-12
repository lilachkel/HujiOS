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

template<typename Key = std::pair<int,int>, typename Data = char *>

class FbrAlgorithm : public ICacheAlgorithm<Key, Data>
{
    using Base = ICacheAlgorithm<Key, Data>;
    virtual void Update(typename CacheMap<Key, Data>::iterator &cm);
    template<typename K = std::pair<int, int>, typename D = void *>
    LruAlgorithm<K, D> *new_Lru = nullptr;
    template<typename K = std::pair<int, int>, typename D = void *> LruAlgorithm<K, D> *m_Lru = nullptr;

    template<typename K = std::pair<int, int>, typename D = void *>
    LfuAlgorithm<K, D> *old_Lfu = nullptr;

    bool m_exist;

    void FreeFbrNode(FbrNode node)
    {
        free(node._blockBuff);
        free(&node);
    }

public:
    FbrAlgorithm(size_t size, double f_old, double f_new );

    ~FbrAlgorithm()
    {

    }

    virtual void RemoveByFileID(int fd);

    virtual Data Get(Key key);

    virtual int Set(Key key, Data data);

    virtual void PrintCache(FILE *f, std::unordered_map<int, std::string> &files);

};


#endif //PROJECT_FBRALGORITHM_H
