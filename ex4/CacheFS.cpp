#include <cstddef>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "CacheFS.h"
#include "ICacheAlgorithm.hpp"
#include "LruAlgorithm.h"
#include "LfuAlgorithm.h"
#include <bitset>
#include <cstring>
#include <unordered_set>

#define RET_FAILURE -1
#define RET_SUCCESS 0


template<typename K = std::pair<int, int>, typename D = void *>
ICacheAlgorithm<K, D> *_algorithm = nullptr;
std::unordered_map<int, std::string> _openFiles;

int cache_hits, cache_misses;

/**
 * Gets the current blocks size.
 * @return block size unsigned long
 */
size_t GetBlockSize()
{
    struct stat fi;
    stat("/tmp", &fi);
    return fi.st_blksize;
}

int CacheFS_init(int blocks_num, cache_algo_t cache_algo, double f_old, double f_new)
{
    if (blocks_num <= 0)
        return RET_FAILURE;

    cache_hits = 0;
    cache_misses = 0;

    switch (cache_algo)
    {
        case LRU:
            _algorithm<std::pair<int, int>, void *> =
                    new LruAlgorithm<std::pair<int, int>, void *>(GetBlockSize() * blocks_num);
            break;
        case LFU:
            _algorithm<std::pair<int, int>, void *> =
                    new LfuAlgorithm<std::pair<int, int>, void *>(GetBlockSize() * blocks_num);
            break;
        case FBR:
            if (f_new < 0 || f_new > 1 || f_old < 0 || f_old > 1 || (f_new + f_old) > 1)
                return RET_FAILURE;
//            _algorithm<int, char *> = new FbrAlgorithm<int, char *>(GetBlockSize() * blocks_num);
            break;
        default:
            return RET_FAILURE;
    }
    return RET_SUCCESS;
}

int CacheFS_destroy()
{
    try
    {
        delete _algorithm<std::pair<int, int>, void *>;
    }
    catch (std::exception &e)
    {
        return RET_FAILURE;
    }

    return RET_SUCCESS;
}

int CacheFS_open(const char *pathname)
{
    auto path = realpath(pathname, NULL);
    int pos = std::string(path).find("/tmp");
    if (pos == std::string::npos || pos != 0)
        return RET_FAILURE;
    int _id = open(path, O_RDONLY | O_DIRECT | O_SYNC);
    if (_id != -1)
        _openFiles.insert({_id, std::string(pathname)});
    return _id;
}

int CacheFS_close(int file_id)
{
    try
    {
        _algorithm<std::pair<int, int>, void *>->RemoveByFileID(file_id);
        close(file_id);
        _openFiles.erase(file_id);
    }
    catch (std::exception e)
    {
        return RET_FAILURE;
    }

    return RET_SUCCESS;
}

int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset)
{

    if (offset < 0 || !buf)
    {
        return RET_FAILURE;

    }
    try//checks, in O(1) if this file_id is valid
    {
        _openFiles.at(file_id);
    }
    catch (std::exception e)
    {
        return RET_FAILURE;
    }
//    lseek(file_id, offset, SEEK_SET);
    size_t blockSize = GetBlockSize();
//    off_t fsize = lseek(file_id, 0, SEEK_END);
    size_t junkBits = offset % blockSize;
    size_t buf_offset = 0;
    size_t addToOffset = 0;

    void *_cacheBuff = aligned_alloc(blockSize, blockSize);//correct? block size buff
    size_t cur_count = count;
    int blockCandid = (int) (offset / blockSize);

    while (cur_count > 0)
    {
        std::pair<int, int> key = std::make_pair(file_id, blockCandid);
        if ((_cacheBuff = _algorithm<std::pair<int, int>, void *>->Get(key)) != nullptr)
        {
            addToOffset = std::min(blockSize - junkBits, count);
            memcpy(buf + buf_offset, _cacheBuff + junkBits,
                   addToOffset); // buff cur offset, the wanted block part, size to copy

        }
        else
        {

            ssize_t _readSize = pread(file_id, _cacheBuff, blockSize, blockCandid * blockSize);
            if (_readSize == -1)
            {
                return RET_FAILURE;
            }

            if (_readSize == 0)
            {
                break;
            }
            _algorithm<std::pair<int, int>, void *>->Set(key, _cacheBuff);
            addToOffset = _readSize - junkBits;
            memcpy(buf + buf_offset, _cacheBuff + junkBits, addToOffset);
            _cacheBuff = aligned_alloc(blockSize, blockSize);
        }
        buf_offset += addToOffset;
        junkBits = 0;
        blockCandid += blockSize;
//        _curOffset += addToOffset;
        cur_count -= addToOffset;
    }

    return (int) buf_offset;
}

int CacheFS_print_cache(const char *log_path)
{
    auto path = realpath(log_path, NULL);
    FILE *f = fopen(path, "a");

    _algorithm<std::pair<int, int>, void *>->PrintCache(f, _openFiles);

    fflush(f);
    fclose(f);
    return RET_SUCCESS;
}

int CacheFS_print_stat(const char *log_path)
{
    auto path = realpath(log_path, NULL);
    FILE *f = fopen(path, "a");

    fprintf(f, "Hits number: %d\nMisses number: %d\n", cache_hits, cache_misses);

    fflush(f);
    fclose(f);
    return RET_SUCCESS;
}
