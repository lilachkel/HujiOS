//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LFUALGORITHM_H
#define PROJECT_LFUALGORITHM_H

#include "ICacheAlgorithm.hpp"

struct LfuNode
{
    int count;
    LfuNode *prev, *next;
    std::list<int> keys;

    LfuNode() : count(1), prev(nullptr), next(nullptr)
    {}

    LfuNode(int freq) : count(freq), prev(nullptr), next(nullptr)
    {}
};

template<typename Key = int, typename Data = char *>
class LfuAlgorithm : public ICacheAlgorithm<Key, Data>
{
    using Base = ICacheAlgorithm<Key, Data>;

    LfuNode *_head;

    std::unordered_map<Key, LfuNode *> _lfu;

    virtual void Update(typename CacheMap<Key, Data>::iterator &cm);

    void updateHead(int key);

    void removeNode(LfuNode *node);

    void removeOldNode();

public:
    LfuAlgorithm(size_t size) : ICacheAlgorithm<Key, Data>(size), _head(nullptr)
    {}

    ~LfuAlgorithm()
    {
        delete _head;
    }

    virtual Data Get(Key key);

    virtual int Set(Key key, Data page);
};


#endif //PROJECT_LFUALGORITHM_H
