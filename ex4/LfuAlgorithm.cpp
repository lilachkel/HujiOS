//
// Created by jenia90 on 6/4/17.
//

#include "LfuAlgorithm.h"

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::Update(typename CacheMap<Key, Data>::iterator &cm)
{
    // Get the key
    int key = cm->first;
    // get the node we are working with
    LfuNode<Key> *node = _lfu[key];
    // delete the key from node's list of keys.
    node->keys.erase(cm->second.second);

    // if node doesnt have next node we add a new node with a higher access count. Then add the key to that item's list.
    if (node->next == nullptr)
    {
        node->next = new LfuNode<Key>(node->count + 1);
        node->next->prev = node;
        node->next->keys.push_back(key);
    }
        // if node has a next node we add the key to it
    else if (node->next->count == node->count + 1)
    {
        node->next->keys.push_back(key);
    }
        // final case is for when the node has more nodes we insert a new node to that list and add the key to it.
    else
    {
        LfuNode<Key> *newNode = new LfuNode<Key>(node->count + 1);
        newNode->next = node->next;
        node->next->prev = newNode;
        newNode->prev = node;
        node->next = newNode;
        node->next->keys.push_back(key);
    }
    // update the key's position
    cm->second.second = std::prev(node->next->keys.end());

    // add the <key, node> to the lfu cache
    _lfu.insert({key, node->next});

    // if the original node doesn't hold any keys - remove it.
    if (node->keys.empty())
        removeNode(node);
}

template<typename Key, typename Data>
Data LfuAlgorithm<Key, Data>::Get(Key key)
{
    // if the key doesn't exist return null.
    auto item = Base::_cache.find(key);
    if (item == Base::_cache.end())
    {
        return nullptr;
    }

    // update the key access frequency and return it's data.
    Update(item);
    return item->second.first;
}

template<typename Key, typename Data>
int LfuAlgorithm<Key, Data>::Set(Key key, Data data)
{
    // if the key already exists update it's access frequency and replace it's data.
    auto item = Base::_cache.find(key);
    if (item != Base::_cache.end())
    {
        Update(item);
        item->second.first = data;
    }

    else
    {
        // if the key doesn't exist and there's no more room in the buffer - remove the LFU node first.
        if (Base::_cache.size() == Base::_capacity)
        {
            removeOldNode();
        }

        // add the key to the head because it has the lowest access count.
        updateHead(key);
        // add the key to the cache buffer.
        Base::_cache.insert({key, {data, std::prev(_head->keys.end())}});
    }
    return 0;
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::updateHead(Key key)
{
    // if head is null - initialize it and add the key to it.
    if (_head == nullptr)
    {
        _head = new LfuNode<Key>();
        _head->keys.push_back(key);
    }
        // if _head frequncy is 1 we add the key to it
    else if (_head->count == 1)
    {
        _head->keys.push_back(key);
    }
        // if head's frequency is higher than 1 - create a new head and add link the current head as it's next.
    else
    {
        LfuNode<Key> *newNode = new LfuNode<Key>();
        _head->prev = newNode;
        newNode->next = _head;
        _head = newNode;
        _head->keys.push_back(key);
    }

    // add the new <key, _head node> pair to the LFU queue.
    _lfu.insert({key, _head});
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::removeNode(LfuNode<Key> *node)
{
    // if it has next node then relink next node's prev to the prev of the current node
    if (node->next != nullptr)
        node->next->prev = node->prev;

    // if it has prev node - relink prev node's next pointer to the next pointer of the current node.
    if (node->prev != nullptr)
        node->prev->next = node->next;

        // if it doesnt have a previous node (node is the head) - make the next node the new head and delete the current
        // head.
    else
    {
        LfuNode<Key> *next = _head->next;
        delete _head;
        _head = next;
        return;
    }

    delete node;
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::removeOldNode()
{
    // if head is already null then do nothing.
    if (_head == nullptr) return;
    Key old = Key();
    // if head's key list is not empty get the key and remove it from the head.
    if (!_head->keys.empty())
    {
        old = _head->keys.front();
        _head->keys.pop_front();
    }
    // if head is empty now - remove it.
    if (_head->keys.empty()) removeNode(_head);

    // remove the old key from the buffer
    free(Base::_cache[old].first);
    Base::_cache.erase(old);
    // remove the old key from the lfu queue.
    _lfu.erase(old);
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::RemoveByFileID(int fd)
{
    for (auto iter = Base::_cache.begin(); iter != Base::_cache.end(); iter++)
    {
        if (iter->first.first == fd)
        {
            _lfu.erase(iter->first);
            // get the node we are working with
            LfuNode<Key> *node = _lfu[iter->first];
            free(iter->second.first);
            // delete the key from node's list of keys.
            node->keys.erase(iter->second.second);
        }
    }
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::PrintCache(FILE *f)
{
    PrintHelper(f, _head);
}

template<typename Key, typename Data>
void LfuAlgorithm<Key, Data>::PrintHelper(FILE *f, LfuNode<Key> *node)
{
    if (node == nullptr)
        return;
    PrintHelper(f, node->next);

    char path[255];
    for (auto &i : node->keys)
    {
        readlink(("/proc/self/fd/" + std::string(i->first)).c_str(), path, 255);
        fprintf(f, "%s %d\n", path, i->second);
    }
}