
#include "FbrAlgorithm.h"

template<typename Key, typename Data>
FbrAlgorithm<Key, Data>::FbrAlgorithm(size_t size, double f_old, double f_new ) :  ICacheAlgorithm<Key, Data>(size)
 {
    new_Lru = new LruAlgorithm<Key, Data>(size*((size_t)f_new));
    old_Lfu = new LfuAlgorithm<Key, Data>(size*((size_t)f_old));
    m_exist = (1-(f_new+f_old) != 0);
    if (m_exist)
    {
        m_Lru = new LruAlgorithm<std::pair<int, int>, void *>(size*((size_t)(1-(f_new+f_old))));
    }
}


template<typename Key, typename Data>
FbrAlgorithm<Key, Data>::~FbrAlgorithm()
{
    delete new_Lru;
    delete m_Lru;
    delete old_Lfu;
}

template<typename Key, typename Data>
Data FbrAlgorithm<Key, Data>::Get(Key key)
{
    FbrNode* node = new_Lru->Get(key);
    // checks if new contains the  key, if true: regular lru get.
    if(node != nullptr)//correct?
    {
        return node->_blockBuff;
    }
    std::pair<Key, FbrNode*> _block;
    if (m_exist)
    {
        _block = m_Lru->FbrGet(key);
        //checks if M contains the  key, if true: count++, and set it in the new list.
        if(_block.first)
        {
            _block.second->_count++;
            SetNew(_block.first, _block.second);
            return _block.second->_blockBuff;
        }
    }
    _block = old_Lfu->FbrGet(key);
    if(_block.first)
    {
        _block.second->_count++;
        SetNew(_block.first, _block.second);
        return _block.second->_blockBuff;
    }
    return nullptr;
}

template <typename Key, typename Data>
void FbrAlgorithm<Key,Data>::SetNew(Key key,FbrNode* node)
{
    node->_type = NEW;
    std::pair<Key, FbrNode*> block = new_Lru->FbrSet(key,node);
    if (block.first != nullptr)
    {
        if (m_exist)
        {
            SetM(block);
        } else
        {
            SetOld(block);
        }
    }
};


template<typename Key, typename Data>
void FbrAlgorithm<Key,Data>::SetM(std::pair<Key, FbrNode*> block)
{
    block.second->_type = MIDDLE;
    std::pair<Key, FbrNode*> _block = m_Lru->FbrSet(block.first, block.second);
    if (_block.first != nullptr)
    {
        SetOld(_block);
    }
};

template<typename Key, typename Data>
void FbrAlgorithm<Key,Data>::SetOld(std::pair<Key, FbrNode*> block)
{
    block.second->_type = OLD;
    old_Lfu->Set(block.first, block.second, block.second->_count, nullptr, &FreeFbrNode);
};

template<typename Key, typename Data>
int FbrAlgorithm<Key, Data>::Set(Key key, Data page)
{
    FbrNode* node = new FbrNode((char*)page);
    SetNew(key, node);
    return 0;
};


template<typename Key, typename Data>
void FbrAlgorithm<Key, Data>::PrintCache(FILE *f)
{
    new_Lru->PrintCache(f);
    if (m_exist)
    {
        m_Lru->PrintCache(f);
    }
    old_Lfu->PrintCache(f);
}