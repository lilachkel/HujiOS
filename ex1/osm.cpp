//
// Created by jenia90 on 3/8/17.
//

#include <zconf.h>
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

void dummyFunc(){}

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
        timemeasure += e.tv_usec - s.tv_usec;
    }
    if(timemeasure != 0)
    {
        return (timemeasure * 1000) / iterations;
    }
    return -1;
}

double osm_function_time(unsigned int iterations)
{
    struct timeval s, e;
    double diff = 0;

    for(int i = 0; i < iterations; i++)
    {
        gettimeofday(&s, nullptr);
        dummyFunc();
        gettimeofday(&e, nullptr);
        diff += e.tv_usec - s.tv_usec;
    }

    return (diff * 1000) / iterations;
}

double osm_syscall_time(unsigned int iterations)
{
    return 0;
}

double osm_disk_time(unsigned int iterations)
{
    return 0;
}

timeMeasurmentStructure measureTimes(unsigned int operation_iterations,
                                     unsigned int function_iterations,
                                     unsigned int syscall_iterations,
                                     unsigned int disk_iterations)
{
    timeMeasurmentStructure time_s;
    char *a = "blah";
    double  iTime = osm_operation_time(operation_iterations),
            fTime = osm_function_time(function_iterations),
            dTime = osm_disk_time(disk_iterations),
            sTime = osm_syscall_time(syscall_iterations),
            ifTime = iTime == 0 || fTime == 0 ? 0 : iTime / fTime,
            idTime = iTime == 0 || dTime == 0 ? 0 : iTime / dTime,
            isTime = iTime == 0 || sTime == 0 ? 0 : iTime / sTime;

    gethostname(time_s.machineName, 100);
    time_s.functionInstructionRatio = ifTime;
    time_s.functionTimeNanoSecond = fTime;
    time_s.diskTimeNanoSecond = dTime;
    time_s.diskInstructionRatio = idTime;
    time_s.trapTimeNanoSecond = sTime;
    time_s.trapInstructionRatio = isTime;
    time_s.instructionTimeNanoSecond = iTime;

    return time_s;
}
