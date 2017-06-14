#include "LruAlgorithm.h"

LruAlgorithm::LruAlgorithm(size_t size) : ICacheAlgorithm(size)
{}

LruAlgorithm::~LruAlgorithm()
{
    _lru.clear();
}

DataType LruAlgorithm::Get(KeyType key)
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

std::pair<KeyType, DataType> LruAlgorithm::FbrGet(KeyType key)
{
    auto item = Base::_cache.find(key);
    if (item == Base::_cache.end())
        return std::make_pair(key, nullptr);

    DataType data = item->second.first;

    _lru.erase(item->second.second);
    Base::_cache.erase(key);

    return std::make_pair(key, data);
}

int LruAlgorithm::Set(KeyType key, DataType data)
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

std::pair<KeyType, DataType> LruAlgorithm::FbrSet(KeyType key, DataType data)
{
    int f, s;
    DataType oldData = nullptr;
    KeyType k = {f, s};
    // Check if the item with the given key is already cached
    auto item = Base::_cache.find(key);
    if (item != Base::_cache.end())
    {
        // If cached update its position as if it was accessed and update its data.
        Update(item);
        item->second.first = data;

        return std::make_pair(k, oldData);

    }

    // check if we reached capacity limit
    if (Base::_cache.size() == Base::_capacity)
    {
        f = _lru.back().first;
        s = _lru.back().second;
        k.first = f;
        k.second = s;
        oldData = Base::_cache[{f, s}].first;
        // if yes, evict the LRU item.
        Base::_cache.erase({f, s});
        _lru.pop_back();
    }

    _lru.push_front(key);
    Base::_cache.insert({key, {data, _lru.begin()}});

    return std::make_pair(k, oldData);
}

void LruAlgorithm::Update(CacheMap::iterator &cm)
{
    // remove the item from the queue and add it in the front
    _lru.erase(cm->second.second);
    _lru.push_front(cm->first);

    // update the item's position iterator in the cache map
    cm->second.second = _lru.begin();
}

void LruAlgorithm::PrintCache()
{
    char path[FILENAME_MAX];
    for (auto &item : _lru)
    {
        readlink(("/proc/self/fd/" + std::to_string(item.first)).c_str(), path, FILENAME_MAX);
        std::cout << path << " " << item.second << std::endl;
    }
}
