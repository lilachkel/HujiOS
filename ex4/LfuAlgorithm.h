//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LFUALGORITHM_H
#define PROJECT_LFUALGORITHM_H

#include "ICacheAlgorithm.hpp"
//#include "LfuAlgorithm.cpp"

struct LfuNode
{
    int count;
    LfuNode *prev, *next;
    std::list<KeyType> keys;

    LfuNode() : count(1), prev(nullptr), next(nullptr)
    {}

    LfuNode(int freq) : count(freq), prev(nullptr), next(nullptr)
    {}
};

class LfuAlgorithm : public ICacheAlgorithm
{
    using Base = ICacheAlgorithm;

    LfuNode *_head;

    std::unordered_map<KeyType, LfuNode *, PairHash, PairEqual> _lfu;

    void DestroyLFU(LfuNode *node);

    virtual void Update(CacheMap::iterator &cm);

    /**
     * Update the double linked-list's head with the given key.
     * @param key
     */
    void updateHead(KeyType key);

    std::list<KeyType>::iterator updateExisting(KeyType key, LfuNode *node, int count);

    /**
     * Check if the node has next or previous nodes linked to it and relink everything bypassing the given node.
     * Then, delete given node
     * @param node LfuNode pointer to remove.
     */
    void removeNode(LfuNode *node);

    /**
     * Remove the node with lowest frequency.
     */
    void removeOldNode(void (*freeData)(DataType));

    /**
     * Prints the cache state recursively
     * @param f log file pointer
     * @param node current node in our traversal.
     */
    void PrintHelper(LfuNode *node);

public:
    LfuAlgorithm(size_t size);

    ~LfuAlgorithm();

    virtual DataType Get(KeyType key);

    std::pair<KeyType, DataType> FbrGet(KeyType key);

    virtual int Set(KeyType key, DataType data);

    virtual int Set(KeyType key, DataType data, int count, void (*freeData)(DataType));

    virtual void PrintCache();
};


#endif //PROJECT_LFUALGORITHM_H
