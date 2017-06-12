#include "LruAlgorithm.h"

template<typename Key, typename Data>
LruAlgorithm<Key, Data>::LruAlgorithm(size_t size) : ICacheAlgorithm<Key, Data>(size)
{}

template<typename Key, typename Data>
LruAlgorithm<Key, Data>::~LruAlgorithm()
{
    _lru.clear();
}

template<typename Key, typename Data>
Data LruAlgorithm<Key, Data>::Get(Key key)
{
    // Check if the item is already cached. If not, return nullptr.
    auto item = Base::_cache.find(key);
    if (item == Base::_cache.end())
        return nullptr;

    // Update the item position in LRU queue
    Update(item);

    // return data.
    return item->second.first;
}

template<typename Key, typename Data>
void LruAlgorithm<Key, Data>::FbrTouch(Key key)
{
    auto item = Base::_cache.find(key);
    if (item == Base::_cache.end())
        return;

    _lru.erase(item->second.second);
    Base::_cache.erase(key);
}

template<typename Key, typename Data>
int LruAlgorithm<Key, Data>::Set(Key key, Data data)
{
    // Check if the item with the given key is already cached
    auto item = Base::_cache.find(key);
    if (item != Base::_cache.end())
    {
        // If cached update its position as if it was accessed and update its data.
        Update(item);
        item->second.first = data;

        return 0;
    }

    // check if we reached capacity limit
    if (Base::_cache.size() == Base::_capacity)
    {
        // if yes, evict the LRU item.
        free(Base::_cache[_lru.back()].first);
        Base::_cache.erase(_lru.back());
        _lru.pop_back();
    }

    // insert new item to the cache and update the queue.
    _lru.push_front(key);
    Base::_cache.insert({key, {data, _lru.begin()}});

    return 0;
}

template<typename Key, typename Data>
std::pair<Key, Data> LruAlgorithm<Key, Data>::FbrSet(Key key, Data data)
{
    Key oldKey;
    Data oldData;
    // Check if the item with the given key is already cached
    auto item = Base::_cache.find(key);
    if (item != Base::_cache.end())
    {
        // If cached update its position as if it was accessed and update its data.
        Update(item);
        item->second.first = data;

        return {Key(), nullptr};

    }

    // check if we reached capacity limit
    if (Base::_cache.size() == Base::_capacity)
    {
        oldKey = _lru.back();
        oldData = Base::_cache.erase(oldKey);
        // if yes, evict the LRU item.
        Base::_cache.erase(oldKey);
        _lru.pop_back();
    }

    return {oldKey, oldData};
}

template<typename Key, typename Data>
void LruAlgorithm<Key, Data>::Update(typename CacheMap<Key, Data>::iterator &cm)
{
    // remove the item from the queue and add it in the front
    _lru.erase(cm->second.second);
    _lru.push_front(cm->first);

    // update the item's position iterator in the cache map
    cm->second.second = _lru.begin();
}

template<typename Key, typename Data>
void LruAlgorithm<Key, Data>::PrintCache(FILE *f)
{
    char path[FILENAME_MAX];
    for (auto &item : _lru)
    {
        readlink(("/proc/self/fd/" + std::to_string(item->first)).c_str(), path, FILENAME_MAX);
        fprintf(f, "%s %d\n", path, item->second);
    }
}
