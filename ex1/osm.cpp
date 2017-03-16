#include <fcntl.h>
#include "osm.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/time.h>
#include <malloc.h>
#include "funcplaceholder.h"

#define MICRO_TO_NANO(x) x * 1000

#define NAME_SIZE 255

char name[NAME_SIZE];

int osm_init()
{
    gethostname(name, NAME_SIZE);
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
    size_t i = 0;

    double timemeasure = 0;
    gettimeofday(&s, nullptr);
    for (; i < iterations ; i++)
    {
        op();
    }
    gettimeofday(&e, nullptr);

    timemeasure += e.tv_usec - s.tv_usec;
    if(timemeasure != 0)
    {
        return MICRO_TO_NANO(timemeasure) / iterations;
    }

    return -1;
}

double osm_operation_time(unsigned int iterations)
{
    struct timeval s, e;
    size_t i = 0;
    double timemeasure = 0;

    gettimeofday(&s, nullptr);
    for (; i < iterations ; i+=10)
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
        return MICRO_TO_NANO(timemeasure) / iterations;
    }

    return -1;
}

double osm_function_time(unsigned int iterations)
{
    return measureRuntime(dummyfunc, iterations);
}

double osm_syscall_time(unsigned int iterations)
{
    return measureRuntime([] { OSM_NULLSYSCALL; }, iterations);
}

double osm_disk_time(unsigned int iterations)
{
    double time;
    struct stat fi;
    stat("/tmp", &fi);
    size_t blksize = fi.st_blksize;
    char *p = (char *) aligned_alloc(blksize, blksize);

    // lets create some data in our write buffer.
    for (size_t i = 0; i < blksize; ++i) {
        p[i] = (char) i;
    }

    // lets pass the open/write operation to the measureRuntime function.
    time = measureRuntime([blksize, p] {
        int fd = open(TEST_FNAME, O_DIRECT | O_SYNC);
        write(fd, p, blksize);
        close(fd);
    }, iterations);

    free(p);

    return time;
}

timeMeasurmentStructure measureTimes(unsigned int operation_iterations,
                                     unsigned int function_iterations,
                                     unsigned int syscall_iterations,
                                     unsigned int disk_iterations)
{

    timeMeasurmentStructure time_s;
    time_s.machineName = name;

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
