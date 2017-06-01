#include <cstddef>
#include <fcntl.h>
#include "CacheFS.h"

int CacheFS_init(int blocks_num, cache_algo_t cache_algo, double f_old, double f_new)
{
    switch (cache_algo)
    {
        case LRU:
            break;
        case LFU:
            break;
        case FBR:
            break;
        default:
            exit(EXIT_FAILURE);
    }
    return 0;
}

int CacheFS_destroy()
{
    return 0;
}

int CacheFS_open(const char *pathname)
{
    return 0;
}

int CacheFS_close(int file_id)
{
    return 0;
}

int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset)
{
    return 0;
}

int CacheFS_print_cache(const char *log_path)
{
    return 0;
}

int CacheFS_print_stat(const char *log_path)
{
    return 0;
}
