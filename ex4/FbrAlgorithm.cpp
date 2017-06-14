
#include "FbrAlgorithm.h"


FbrAlgorithm::FbrAlgorithm(size_t size, double f_old, double f_new) : ICacheAlgorithm(0) {
    new_Lru = new LruAlgorithm(((size_t) (f_new * size)));
    old_Lfu = new LfuAlgorithm(((size_t) (size * f_old)));
    m_exist = (1 - (f_new + f_old) != 0);
    if (m_exist)
    {
        m_Lru = new LruAlgorithm(size * ((size_t) (1 - (f_new + f_old))));
    }
}

FbrAlgorithm::~FbrAlgorithm()
{
    delete new_Lru;
    delete m_Lru;
    delete old_Lfu;
}


DataType FbrAlgorithm::Get(KeyType key)
{
    FbrNode *node = static_cast<FbrNode *>(new_Lru->Get(key));
    // checks if new contains the  key, if true: regular lru get.
    if (node != nullptr)//correct?
    {
        return node->_blockBuff;
    }
    if (m_exist)
    {
        auto temp = m_Lru->FbrGet(key);
        auto block_pair = std::make_pair(temp.first, static_cast<FbrNode *>(temp.second));
        //checks if M contains the  key, if true: count++, and set it in the new list.
        if (block_pair.second != nullptr)
        {
            block_pair.second->_count++;
            SetNew(block_pair.first, block_pair.second);
            return block_pair.second->_blockBuff;
        }
    }
    auto temp = old_Lfu->FbrGet(key);
    auto block_pair = std::make_pair(temp.first, static_cast<FbrNode *>(temp.second));
    if (block_pair.second != nullptr)
    {
        block_pair.second->_count++;
        SetNew(block_pair.first, block_pair.second);
        return block_pair.second->_blockBuff;
    }
    return nullptr;
}

void FbrAlgorithm::SetNew(KeyType key, FbrNode *node)
{

    node->_type = NEW;
    auto temp = new_Lru->FbrSet(key ,node);
    auto block_pair = std::make_pair(temp.first, static_cast<FbrNode *>(temp.second));

    if (block_pair.second != nullptr)
    {
        if (m_exist)
        {
            SetM(block_pair.first, block_pair.second);
        }
        else
        {
            SetOld(block_pair.first, block_pair.second);
        }
    }
};


void FbrAlgorithm::SetM(KeyType key, FbrNode *node)
{
    node->_type = MIDDLE;
    auto temp = m_Lru->FbrSet(key, node);
    auto block_pair = std::make_pair(temp.first, static_cast<FbrNode *>(temp.second));
    if (block_pair.second != nullptr)
    {
        SetOld(block_pair.first, block_pair.second);
    }
};

void FbrAlgorithm::SetOld(KeyType key, FbrNode *node)
{
    node->_type = OLD;
    old_Lfu->Set(key, node, node->_count, nullptr, FreeFbrNode);
};

int FbrAlgorithm::Set(KeyType key, DataType data)
{
    FbrNode *node = new FbrNode(data);
    SetNew(key, node);
    return 0;
};

void FbrAlgorithm::PrintCache()
{
    new_Lru->PrintCache();
    if (m_exist)
    {
        m_Lru->PrintCache();
    }
    old_Lfu->PrintCache();
}
