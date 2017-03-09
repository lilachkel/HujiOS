//
// Created by jenia90 on 3/8/17.
//

#include "osm.h"

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
    return 0;
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
