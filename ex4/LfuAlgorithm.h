//
// Created by jenia90 on 6/4/17.
//

#ifndef PROJECT_LFUALGORITHM_H
#define PROJECT_LFUALGORITHM_H

#include "ICacheAlgorithm.hpp"

template<typename Key>
struct LfuNode
{
    int count;
    LfuNode *prev, *next;
    std::list<Key> keys;

    LfuNode() : count(1), prev(nullptr), next(nullptr)
    {}

    LfuNode(int freq) : count(freq), prev(nullptr), next(nullptr)
    {}
};

template<typename Key = int, typename Data = char *>
class LfuAlgorithm : public ICacheAlgorithm<Key, Data>
{
    using Base = ICacheAlgorithm<Key, Data>;

    LfuNode<Key> *_head;

    std::unordered_map<Key, LfuNode<Key> *, PairHash<int, int>, PairEqual<int, int>> _lfu;

    virtual void Update(typename CacheMap<Key, Data>::iterator &cm);

    /**
     * Update the double linked-list's head with the given key.
     * @param key
     */
    void updateHead(Key key);

    /**
     * Check if the node has next or previous nodes linked to it and relink everything bypassing the given node.
     * Then, delete given node
     * @param node LfuNode pointer to remove.
     */
    void removeNode(LfuNode<Key> *node);

    /**
     * Remove the node with lowest frequency.
     */
    void removeOldNode();

public:
    LfuAlgorithm(size_t size) : ICacheAlgorithm<Key, Data>(size), _head(nullptr)
    {}

    ~LfuAlgorithm()
    {
        delete _head;
    }

    virtual void RemoveByFileID(int fd);

    virtual Data Get(Key key);

    virtual int Set(Key key, Data data);

    virtual void PrintCache(FILE *f);
};


#endif //PROJECT_LFUALGORITHM_H
