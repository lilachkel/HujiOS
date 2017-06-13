
#include "FbrAlgorithm.h"


FbrAlgorithm::FbrAlgorithm(size_t size, double f_old, double f_new) : ICacheAlgorithm(0) {
    new_Lru = new LruAlgorithm(size * ((size_t) f_new));
    old_Lfu = new LfuAlgorithm(size * ((size_t) f_old));
    m_exist = (1 - (f_new + f_old) != 0);
    if (m_exist)
    {
        m_Lru = new LruAlgorithm(size * ((size_t) (1 - (f_new + f_old))));
    }
}

//template<typename Key, typename Data>
FbrAlgorithm::~FbrAlgorithm()
{
    delete new_Lru;
    delete m_Lru;
    delete old_Lfu;
}


//template<typename Key, typename Data>
DataType FbrAlgorithm::Get(KeyType key)
{
    FbrNode *node = (FbrNode *) new_Lru->Get(key);
    // checks if new contains the  key, if true: regular lru get.
    if (node != nullptr)//correct?
    {
        return node->_blockBuff;
    }
    std::pair<KeyType, FbrNode *> *_block;
    if (m_exist)
    {
        //TODO: fix this
        auto temp = m_Lru->FbrGet(key);
        auto block_pair = std::make_pair(temp.first, (FbrNode*)temp.second);
        _block = &block_pair;
        //checks if M contains the  key, if true: count++, and set it in the new list.
        if (_block->second != nullptr)
        {
            _block->second->_count++;
            SetNew(_block->first, _block->second);
            return _block->second->_blockBuff;
        }
    }
    auto temp = old_Lfu->FbrGet(key);
    auto block_pair = std::make_pair(temp.first, (FbrNode*)temp.second);
    _block = &block_pair;
    if (_block->second != nullptr)
    {
        _block->second->_count++;
        SetNew(_block->first, _block->second);
        return _block->second->_blockBuff;
    }
    return nullptr;
}

//template<typename Key, typename Data>
void FbrAlgorithm::SetNew(KeyType key, FbrNode *node)
{
    node->_type = NEW;
    auto temp = new_Lru->FbrSet(key ,node);
    auto block_pair = std::make_pair(temp.first, (FbrNode*)temp.second);
    std::pair<KeyType, FbrNode *> *block = &block_pair;
//    block = &block_pair;

    if (block_pair.second != nullptr)
    {
        if (m_exist)
        {
            SetM(block_pair);
        }
        else
        {
            SetOld(block_pair);
        }
    }
};


//template<typename Key, typename Data>
void FbrAlgorithm::SetM(std::pair<KeyType, FbrNode *> block)
{
    block.second->_type = MIDDLE;
//    auto temp = new_Lru->FbrSet(key ,node);
//    auto block_pair = std::make_pair(temp.first, (FbrNode*)temp.second);
//    std::pair<KeyType, FbrNode *> *block = &block_pair;
    auto temp = m_Lru->FbrSet(block.first, block.second);
    auto block_pair = std::make_pair(temp.first, (FbrNode*)temp.second);
    if (block_pair.second != nullptr)
    {
        SetOld(block_pair);
    }
};

//template<typename Key, typename Data>
void FbrAlgorithm::SetOld(std::pair<KeyType, FbrNode *> block)
{
    block.second->_type = OLD;
    old_Lfu->Set(block.first, block.second, block.second->_count, nullptr, FreeFbrNode);
};

//template<typename Key, typename Data>
int FbrAlgorithm::Set(KeyType key, DataType data)
{
    FbrNode *node = new FbrNode((char *) data);
    SetNew(key, node);
    return 0;
};


//template<typename Key, typename Data>
void FbrAlgorithm::PrintCache(FILE *f)
{
    new_Lru->PrintCache(f);
    if (m_exist)
    {
        m_Lru->PrintCache(f);
    }
    old_Lfu->PrintCache(f);
}
