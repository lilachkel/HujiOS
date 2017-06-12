
#include "FbrAlgorithm.h"
#include "LruAlgorithm.h"
#include "LfuAlgorithm.h"

template<typename Key, typename Data>
FbrAlgorithm<Key, Data>::FbrAlgorithm(size_t size, double f_old, double f_new ) :  ICacheAlgorithm<Key, Data>(size)
 {
//     _f_old = f_old;
//     _f_new = f_new;
    new_Lru<std::pair<int, int>, void *> = new LruAlgorithm<std::pair<int, int>, void *>(size*((size_t)f_new));
    old_Lfu<std::pair<int, int>, void *> = new LfuAlgorithm<std::pair<int, int>, void *>(size*((size_t)f_old));
    m_exist = (1-(f_new+f_old) != 0);
    if (m_exist)
    {
        m_Lru<std::pair<int, int>, void *> = new LruAlgorithm<std::pair<int, int>, void *>(size*((size_t)(1-(f_new+f_old))));
    }

}


template<typename Key, typename Data>
Data FbrAlgorithm<Key, Data>::Get(Key key)
{
    FbrNode node = new_Lru->Get(key);
    if(!&node)//correct?
    {
        return node._blockBuff;
    }
    std::pair<Key, FbrNode> _block = m_Lru->FbrGet(key);
    if(_block.first)
    {
//        _block.
    }

}


template<typename Key, typename Data>
int FbrAlgorithm<Key, Data>::Set(Key key, Data page)
{
//    Base::_cache.insert({key, new FbrNode(page) });
    std::pair<Key, FbrNode> _block = new_Lru->FbrSet(key, new FbrNode((char*)page));
    if(_block.first != nullptr)
    {
        _block = m_Lru->FbrSet(_block.first, _block.second);
        if (_block.first != nullptr)
        {
            old_Lfu->Set(_block.first, _block.second, _block.second._count, nullptr, &FreeFbrNode);
        }
    }
    return 0;

};


template<typename Key, typename Data>
void FbrAlgorithm<Key, Data>::Update(typename CacheMap<Key, Data>::iterator &cm)
{

}

template<typename Key, typename Data>
void FbrAlgorithm<Key, Data>::PrintCache(FILE *f, std::unordered_map<int, std::string> &files)
{

}