//
// Created by jenia90 on 3/8/17.
//
#define _ISOC11_SOURCE

#include <cstdio>
#include <fcntl.h>
#include "osm.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <zconf.h>
#include <functional>
#include <sys/time.h>
#include <malloc.h>
#include "funcplaceholder.h"


int osm_init()
{
    return 0;
}

int osm_finalizer()
{
    return 0;
}

template <typename func>
double measureRuntime(func op, unsigned int iterations)
{
    struct timeval s, e;
    int i = 0;

    double timemeasure = 0;
    gettimeofday(&s, nullptr);
    for (i; i < iterations ; i++)
    {
        op();
    }
    gettimeofday(&e, nullptr);

    timemeasure += e.tv_usec - s.tv_usec;
    if(timemeasure != 0)
    {
        return (timemeasure * 1000) / iterations;
    }

    return -1;
}

double osm_operation_time(unsigned int iterations)
{
    struct timeval s, e;
    int i = 0;

    double timemeasure = 0;
    gettimeofday(&s, nullptr);
    for (i; i < iterations ; i+=10)
    {
        i += 0;
        i += 0;
        i += 0;
        i += 0;
        i += 0;
        i += 0;
        i += 0;
        i += 0;
        i += 0;
        i += 0;
    }
    gettimeofday(&e, nullptr);

    timemeasure += e.tv_usec - s.tv_usec;
    if(timemeasure != 0)
    {
        return (timemeasure * 1000) / iterations;
    }

    return -1;
}

double osm_function_time(unsigned int iterations)
{
    return measureRuntime(dummyfunc, iterations);
}

double osm_syscall_time(unsigned int iterations)
{
    std::function<void ()> f = [] { OSM_NULLSYSCALL; };
    return measureRuntime(f, iterations);
}

double osm_disk_time(unsigned int iterations)
{
    struct stat fi;
    stat("/tmp", &fi);
    int blksize = fi.st_blksize;
    char *p = (char *) aligned_alloc(blksize, blksize);

    for (int i = 0; i < blksize; ++i)
    {
        p[i] = (char)i;
    }

    std::function<void ()> f = [=]
    {
        int id = open("/tmp/tik/someKovez", O_CREAT | O_DIRECT | O_SYNC);
        write(id, p, blksize);
        close(id);
    };

    return measureRuntime(f, iterations);
}

timeMeasurmentStructure measureTimes(unsigned int operation_iterations,
                                     unsigned int function_iterations,
                                     unsigned int syscall_iterations,
                                     unsigned int disk_iterations)
{

    timeMeasurmentStructure time_s;
    time_s.machineName = (char *) malloc(255);

    gethostname(time_s.machineName, 255);
    double  iTime = osm_operation_time(operation_iterations),
            fTime = osm_function_time(function_iterations),
            dTime = osm_disk_time(disk_iterations),
            sTime = osm_syscall_time(syscall_iterations),
            ifTime = iTime == 0 || fTime == 0 ? 0 : fTime / iTime,
            idTime = iTime == 0 || dTime == 0 ? 0 : dTime / iTime,
            isTime = iTime == 0 || sTime == 0 ? 0 : sTime / iTime;

    time_s.functionInstructionRatio = ifTime;
    time_s.functionTimeNanoSecond = fTime;
    time_s.diskTimeNanoSecond = dTime;
    time_s.diskInstructionRatio = idTime;
    time_s.trapTimeNanoSecond = sTime;
    time_s.trapInstructionRatio = isTime;
    time_s.instructionTimeNanoSecond = iTime;

    return time_s;
}
