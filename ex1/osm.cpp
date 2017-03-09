//
// Created by jenia90 on 3/8/17.
//

#include "osm.h"
#include "sys/time.h"
int osm_init()
{
    return 0;
}

int osm_finalizer()
{
    return 0;
}

double osm_operation_time(unsigned int iterations)
{
    struct timeval s, e;
    int i = 0;

    double timemeasure = 0;
    for (i; i<iterations ; i++)
    {
        gettimeofday(&s, nullptr);
        1+1;
        gettimeofday(&e, nullptr);
        timemeasure += e.tv_usec-s.tv_usec;
    }
    if(timemeasure != 0)
    {
        return timemeasure/iterations;
    }
    return -1;
}

double osm_function_time(unsigned int iterations)
{
    return 0;
}

double osm_syscall_time(unsigned int iterations)
{
    return 0;
}

double osm_disk_time(unsigned int iterations)
{
    return 0;
}

timeMeasurmentStructure
measureTimes(unsigned int operation_iterations, unsigned int function_iterations,
             unsigned int syscall_iterations, unsigned int disk_iterations)
{
    return timeMeasurmentStructure();
}
