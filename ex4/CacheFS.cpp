//#define NDEBUG

#include <cstddef>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "CacheFS.h"
#include "ICacheAlgorithm.hpp"
#include "LruAlgorithm.h"
#include "FbrAlgorithm.h"
#include <bitset>
#include <cstring>
#include <unordered_set>
#include <fstream>

#define RET_FAILURE -1
#define RET_SUCCESS 0

ICacheAlgorithm *_algorithm = nullptr;
std::unordered_map<int, std::string> _openFiles;

int cache_hits, cache_misses;


/**
 * Gets the current blocks size.
 * @return block size unsigned long
 */
const size_t GetBlockSize()
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
            _algorithm = new LruAlgorithm(blocks_num);
            break;
        case LFU:
            _algorithm = new LfuAlgorithm(blocks_num);
            break;
        case FBR:
            if (f_new < 0 || f_new > 1 || f_old < 0 || f_old > 1 || (f_new + f_old) > 1)
                return RET_FAILURE;
            _algorithm = new FbrAlgorithm(blocks_num, f_old, f_new);
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
        delete _algorithm;
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
    if (path == NULL)
        return RET_FAILURE;
    size_t pos = std::string(path).find("/tmp");
    if (pos == std::string::npos || pos != 0)
        return RET_FAILURE;
    int fd = open(path, O_RDONLY | O_DIRECT | O_SYNC);
    if (fd != -1)
        _openFiles.insert({fd, std::string(pathname)});
    return fd;
}

int CacheFS_close(int file_id)
{
    try
    {
        _openFiles.at(file_id);
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

    //checks, in O(1) if this file_id is valid
    try
    {
        _openFiles.at(file_id);
    }
    catch (std::exception e)
    {
        return RET_FAILURE;
    }
    size_t blockSize = GetBlockSize();
    size_t junkBits = offset % blockSize;
    size_t buf_offset = 0;
    size_t addToOffset = 0;

    long cur_count = count;
    int blockCandid = (int) (offset / blockSize);
    void *_cacheBuff;
    while (cur_count > 0)
    {
        std::pair<std::string, int> key = std::make_pair(_openFiles[file_id], blockCandid);
        if ((_cacheBuff = _algorithm->Get(key)) != nullptr)
        {
            cache_hits++;
            addToOffset = std::min(strlen((char *) _cacheBuff + junkBits), (size_t) cur_count);
            // buff cur offset, the wanted block part, size to copy
            memcpy((char *) buf + buf_offset, (char *) _cacheBuff + junkBits, addToOffset);

        }
        else
        {
            cache_misses++;
            _cacheBuff = aligned_alloc(blockSize, blockSize);

            ssize_t _readSize = pread(file_id, (char *) _cacheBuff, blockSize, blockCandid * blockSize);
            if (_readSize == -1)
            {
                free(_cacheBuff);
                return RET_FAILURE;
            }

            if (_readSize == 0)
            {
                break;
            }
            _algorithm->Set(key, _cacheBuff);
            addToOffset = std::min(_readSize - junkBits, (size_t)cur_count);
            memcpy((char *) buf + buf_offset, (char *) _cacheBuff + junkBits, addToOffset);
        }
        buf_offset += addToOffset;
        junkBits = 0;
        blockCandid++;
        cur_count -= addToOffset;
    }

    return (int) buf_offset;
}

int CacheFS_print_cache(const char *log_path)
{
#ifdef NDEBUG
    std::ofstream _log;
    _log.open(log_path, std::ios::app | std::ios::out);
    if(_log.fail())
        return RET_FAILURE;

    auto backup = std::cout.rdbuf();
    std::cout.rdbuf(_log.rdbuf());
#endif

    _algorithm->PrintCache();

#ifdef NDEBUG
    _log.flush();
    _log.close();
    std::cout.rdbuf(backup);
#endif
    return RET_SUCCESS;
}

int CacheFS_print_stat(const char *log_path)
{
#ifdef NDEBUG
    std::ofstream _log;
    _log.open(log_path, std::ios::app | std::ios::out);
    if(_log.fail())
        return RET_FAILURE;

    auto backup = std::cout.rdbuf();
    std::cout.rdbuf(_log.rdbuf());
#endif

    std::cout << "Hits number: " << cache_hits << std::endl;
    std::cout << "Misses number: " << cache_misses << std::endl;

#ifdef NDEBUG
    _log.flush();
    _log.close();
    std::cout.rdbuf(backup);
#endif
    return RET_SUCCESS;
}