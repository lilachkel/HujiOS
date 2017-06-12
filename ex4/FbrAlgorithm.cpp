
#include "FbrAlgorithm.h"

template<typename Key, typename Data>
FbrAlgorithm<Key, Data>::~FbrAlgorithm()
{
//    CleanCache(Base::_cache);
}


template<typename Key, typename Data>
Data FbrAlgorithm<Key, Data>::Get(Key key)
{

}


template<typename Key, typename Data>
int FbrAlgorithm<Key, Data>::Set(Key key, Data page) {
}
};


template<typename Key, typename Data>
void FbrAlgorithm<Key, Data>::Update(typename CacheMap<Key, Data>::iterator &cm)
{
}



template<typename Key, typename Data>
void FbrAlgorithm<Key, Data>::RemoveByFileID(int fd)
{
}


template<typename Key, typename Data>
void FbrAlgorithm<Key, Data>::PrintCache(FILE *f)
{}