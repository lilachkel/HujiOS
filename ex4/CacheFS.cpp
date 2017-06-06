#include <cstddef>
#include <fcntl.h>
#include <sys/stat.h>
#include "CacheFS.h"
#include "ICacheAlgorithm.hpp"
#include "LruAlgorithm.h"
#include "LfuAlgorithm.h"
#include "FbrAlgorithm.h"
#include <string>

#define RET_FAILURE -1
#define RET_SUCCESS 0

ICacheAlgorithm *_algorithm = nullptr;

int CacheFS_init(int blocks_num, cache_algo_t cache_algo, double f_old, double f_new)
{
    if (f_new < 0 || f_new > 1 || f_old < 0 || f_old > 1 || (f_new + f_old) > 1 || blocks_num <= 0)
        return RET_FAILURE;

    struct stat fi;
    stat("/tmp", &fi);
    size_t blksize = fi.st_blksize;

    switch (cache_algo)
    {
        case LRU:
            _algorithm = new LruAlgorithm(blksize, blocks_num);
            break;
        case LFU:
            _algorithm = new LfuAlgorithm(blksize, blocks_num);
            break;
        case FBR:
            _algorithm = new FbrAlgorithm();
            break;
        default:
            return RET_FAILURE;
    }
    return RET_SUCCESS;
}

int CacheFS_destroy()
{
    return RET_SUCCESS;
}

int CacheFS_open(const char *pathname)
{
    if (std::string(pathname).find("/tmp") == std::string::npos)
        return RET_FAILURE;
    return RET_SUCCESS;
}

int CacheFS_close(int file_id)
{
    return RET_SUCCESS;
}

int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset)
{
    return RET_SUCCESS;
}

int CacheFS_print_cache(const char *log_path)
{
    return RET_SUCCESS;
}

int CacheFS_print_stat(const char *log_path)
{
    return RET_SUCCESS;
}
