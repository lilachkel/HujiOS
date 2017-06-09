//
// Created by jenia90 on 6/4/17.
//

#include "LruAlgorithm.h"


template<typename Key, typename Data>
LruAlgorithm<Key, Data>::~LruAlgorithm()
{
    CleanCache(Base::_cache);
}

template<typename Key, typename Data>
Data LruAlgorithm<Key, Data>::Get(Key key)
{
    auto item = Base::_cache.find(key);
    if (item == Base::_cache.end())
        return nullptr;

    Update(item);
    return item->second.first;
}

template<typename Key, typename Data>
int LruAlgorithm<Key, Data>::Set(Key key, Data page)
{
    auto item = Base::_cache.find(key);
    if (item != Base::_cache.end())
    {
        Update(item);
        item->second.first = page;

        return 0;
    }

    if (Base::_cache.size() == Base::_capacity)
    {
        Base::_cache.erase(_queue.back());
        _queue.pop_back();
    }

    _queue.push_front(key);
    Base::_cache.insert({key, {page, _queue.begin()}});

    return 0;
}

template<typename Key, typename Data>
void LruAlgorithm<Key, Data>::Update(typename CacheMap<Key, Data>::iterator &cm)
{
    _queue.erase(cm->second.second);
    _queue.push_front(cm->first);
    cm->second.second = _queue.begin();
}

template<typename Key, typename Data>
void LruAlgorithm<Key, Data>::CleanCache(CacheMap<Key, Data> &cm)
{
    for (auto &item : cm)
        delete[] item.second.first;

    cm.clear();
}