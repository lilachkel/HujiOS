//
// Created by jenia90 on 6/4/17.
//

#include "LfuAlgorithm.h"

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::Update(typename CacheMap<Key, Data>::iterator &cm)
{
    int key = cm->first;
    LfuNode *node = _lfu[key];
    node->keys.erase(cm->second.second);

    if (node->next == nullptr)
    {
        node->next = new LfuNode(node->count + 1);
        node->next->prev = node;
        node->next->keys.push_back(key);
    }
    else if (node->next->count == node->count + 1)
    {
        node->next->keys.push_back(key);
    }
    else
    {
        LfuNode *newNode = new LfuNode(node->count + 1);
        newNode->next = node->next;
        node->next->prev = newNode;
        newNode->prev = node;
        node->next = newNode;
        node->next->keys.push_back(key);
    }
    cm->second.second = std::prev(node->next->keys.end());

    _lfu.insert({key, node->next});
    if (node->keys.empty())
    {
        removeNode(node);
    }
}

template<typename Key, typename Data>
Data LfuAlgorithm<Key, Data>::Get(Key key)
{
    auto item = Base::_cache.find(key);
    if (item == Base::_cache.end())
    {
        return nullptr;
    }

    Update(item);
    return item->second.first;
}

template<typename Key, typename Data>
int LfuAlgorithm<Key, Data>::Set(Key key, Data page)
{
    auto item = Base::_cache.find(key);
    if (item != Base::_cache.end())
    {
        Update(item);
        item->second.first = page;
    }

    else
    {
        if (Base::_cache.size() == Base::_capacity)
        {
            removeOldNode();
        }
        updateHead(key);
        Base::_cache.insert({key, {page, std::prev(_head->keys.end())}});
    }
    return 0;
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::updateHead(int key)
{
    if (_head == nullptr)
    {
        _head = new LfuNode(1);
        _head->keys.push_back(key);
    }
    else if (_head->count = 1)
    {
        _head->keys.push_back(key);
    }
    else
    {
        LfuNode *newNode = new LfuNode(1);
        _head->prev = newNode;
        newNode->next = _head;
        _head = newNode;
        _head->keys.push_back(key);
    }
    _lfu.insert({key, _head});
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::removeNode(LfuNode *node)
{
    if (node->next != nullptr)
        node->next->prev = node->prev;

    if (node->prev != nullptr)
        node->prev->next = node->next;

    else
        _head = _head->next;
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::removeOldNode()
{
    if (_head == nullptr) return;
    int old = 0;
    if (!_head->keys.empty())
    {
        old = _head->keys.front();
        _head->keys.pop_front();
    }
    if (_head->keys.empty()) removeNode(_head);
    Base::_cache.erase(old);
    _lfu.erase(old);
}
