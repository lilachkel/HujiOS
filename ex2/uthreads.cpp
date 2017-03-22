//
// Created by jenia90 on 3/22/17.
//

#include "uthreads.h"
#include "Thread.h"
#include <queue>
#include <list>

using namespace std;

int threadCount;
queue<int> freeIds;
list<Thread> Threads;

int GetNextId()
{
    int id;
    if(freeIds.size() > 0)
    {
        id = freeIds.front();
        freeIds.pop();
        return id;
    }

    return threadCount < MAX_THREAD_NUM ? threadCount++ : -1;
}

int uthread_init(int quantum_usecs)
{
    threadCount = 1;
    return 0;
}

int uthread_spawn(void (*f)(void))
{
    int id = GetNextId();
    if (id == -1)
    {
        return id;
    }

    Threads.push_back(Thread(GetNextId(), f, STACK_SIZE));
    return id;
}

int uthread_terminate(int tid)
{
    return 0;
}

int uthread_block(int tid)
{
    return 0;
}

int uthread_resume(int tid)
{
    return 0;
}

int uthread_sync(int tid)
{
    return 0;
}

int uthread_get_tid()
{
    return 0;
}

int uthread_get_total_quantums()
{
    return 0;
}
