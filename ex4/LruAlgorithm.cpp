//
// Created by jenia90 on 6/4/17.
//

#include "LruAlgorithm.h"


char *LruAlgorithm::Get(int key)
{
    auto item = _cache.find(key);
    if (item == _cache.end())
        return nullptr;

    Update(item);
    return item->second.first;
}


int LruAlgorithm::Set(int key, char *page)
{
    auto item = _cache.find(key);
    if (item != _cache.end())
    {
        Update(item);
        //TODO: Check why this doesnt work!
        _cache[key] = {page, _lru.begin()};
        return 0;
    }

    if (_cache.size() == _size)
    {
        _cache.erase(_lru.back());
        _lru.pop_back();
    }

    _lru.push_front(key);
    _cache.insert({key, {page, _lru.begin()}})

    return 0;
}

void LruAlgorithm::Update(CacheMap::iterator &cm)
{
    // TODO: Implement list.splice.
    _lru.erase(cm->second.second);
    _lru.push_front(cm->first);
    cm->second.second = _lru.begin();
}
