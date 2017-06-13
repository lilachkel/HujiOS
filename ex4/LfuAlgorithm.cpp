#include "LfuAlgorithm.h"

//template<typename Key, typename Data>
LfuAlgorithm::LfuAlgorithm(size_t size) : ICacheAlgorithm(size), _head(nullptr)
{}

//template<typename Key, typename Data>
LfuAlgorithm::~LfuAlgorithm()
{
    DestroyLFU(_head);
}

//template<typename Key, typename Data>
void LfuAlgorithm::DestroyLFU(LfuNode *node)
{
    if (node == nullptr)
        return;
    DestroyLFU(node->next);
    delete node;
}

//template<typename Key, typename Data>
void LfuAlgorithm::Update(CacheMap::iterator &cm)
{
    // Get the key
    KeyType key = cm->first;
    // get the node we are working with
    LfuNode *node = _lfu[key];
    // delete the key from node's list of keys.
    node->keys.erase(cm->second.second);

    // if node doesnt have next node we add a new node with a higher access count. Then add the key to that item's list.
    if (node->next == nullptr)
    {
        node->next = new LfuNode(node->count + 1);
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
        LfuNode *newNode = new LfuNode(node->count + 1);
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

//template<typename Key, typename Data>
DataType LfuAlgorithm::Get(KeyType key)
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

//template<typename Key, typename Data>
std::pair<KeyType, DataType> LfuAlgorithm::FbrGet(KeyType key)
{
    auto item = Base::_cache.find(key);
    if (item == Base::_cache.end())
        return std::make_pair(key, nullptr);

    // get the node we are working with
    LfuNode *node = _lfu[key];
    // delete the key from node's list of keys.
    node->keys.erase(item->second.second);

    DataType data = item->second.first;

    Base::_cache.erase(key);

    return std::make_pair(key, data);
}

//template<typename Key, typename Data>
int LfuAlgorithm::Set(KeyType key, DataType data)
{
    Set(key, data, 1, nullptr, free);
}

//template<typename Key, typename Data>
int LfuAlgorithm::Set(KeyType key, DataType data, int count, KeyType *old, void (*freeData)(DataType))
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
        typename std::list<KeyType>::iterator pos;
        // if the key doesn't exist and there's no more room in the buffer - remove the LFU node first.
        if (Base::_cache.size() == Base::_capacity)
        {
            removeOldNode(old, freeData);
        }

        // add the key to the head because it has the lowest access count.
        if (count == 1)
        {
            updateHead(key);
            pos = _head->keys.end();
        }
        else
        {
            pos = updateExisting(key, _head, count);
        }
        // add the key to the cache buffer.
        Base::_cache.insert({key, {data, std::prev(pos)}});
    }
    return 0;
}

//template<typename Key, typename Data>
std::list<KeyType>::iterator LfuAlgorithm::updateExisting(KeyType key, LfuNode *node, int count)
{
    if (node->count == count)
    {
        node->keys.push_back(key);
        return node->keys.end();
    }
        // if we have to add the key in the middle of the list
    else if (node->next != nullptr)
    {
        // there are nodes with higher frequency
        if (node->next->count <= count)
        {
            return updateExisting(key, node->next, count);
        }
        else
        {
            LfuNode *newNode = new LfuNode(count);
            newNode->next = node->next;
            node->next->prev = newNode;
            node->next = newNode;
            newNode->prev = node;
            newNode->keys.push_back(key);
            return newNode->keys.end();
        }
    }

    else
    {
        LfuNode *newNode = new LfuNode(count);
        node->next = newNode;
        newNode->prev = node;
        newNode->keys.push_back(key);
        return newNode->keys.end();
    }
}

//template<typename Key, typename Data>
void LfuAlgorithm::updateHead(KeyType key)
{
    // if head is null - initialize it and add the key to it.
    if (_head == nullptr)
    {
        _head = new LfuNode();
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
        LfuNode *newNode = new LfuNode();
        _head->prev = newNode;
        newNode->next = _head;
        _head = newNode;
        _head->keys.push_back(key);
    }

    // add the new <key, _head node> pair to the LFU queue.
    _lfu.insert({key, _head});
}

//template<typename Key, typename Data>
void LfuAlgorithm::removeNode(LfuNode *node)
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
        LfuNode *next = _head->next;
        delete _head;
        _head = next;
        return;
    }

    delete node;
}

//template<typename Key, typename Data>
void LfuAlgorithm::removeOldNode(KeyType *oldKey, void (*freeData)(DataType))
{
    // if head is already null then do nothing.
    if (_head == nullptr) return;
    KeyType *old = new KeyType();
    // if head's key list is not empty get the key and remove it from the head.
    if (!_head->keys.empty())
    {
        old = &_head->keys.front();
        oldKey = old;
        _head->keys.pop_front();
    }
    // if head is empty now - remove it.
    if (_head->keys.empty()) removeNode(_head);

    // remove the old key from the buffer
    freeData(Base::_cache[*old].first);
    Base::_cache.erase(*old);
    // remove the old key from the lfu queue.
    _lfu.erase(*old);
}

//template<typename Key, typename Data>
void LfuAlgorithm::PrintCache(FILE *f)
{
    PrintHelper(f, _head);
}

//template<typename Key, typename Data>
void LfuAlgorithm::PrintHelper(FILE *f, LfuNode *node)
{
    if (node == nullptr)
        return;
    PrintHelper(f, node->next);

    char path[FILENAME_MAX];
    for (auto &i : node->keys)
    {
        readlink(("/proc/self/fd/" + std::to_string(i.first)).c_str(), path, FILENAME_MAX);
        fprintf(f, "%s %d\n", path, i.second);
    }
}
