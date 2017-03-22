//
// Created by jenia90 on 3/22/17.
//

#include "uthreads.h"
#include "Thread.h"
#include <queue>
#include <map>
#include <list>

using namespace std;

int _threadCount, _runningTID, _qtime, _quantum_usecs;
queue<int> _freeIds;
list<int> _roundRobinQueue;
map<int, Thread> _threads;


int GetNextId()
{
    int id;
    if(_freeIds.size() > 0)
    {
        id = _freeIds.front();
        _freeIds.pop();
        return id;
    }

    return _threadCount < MAX_THREAD_NUM ? _threadCount++ : -1;
}

int uthread_init(int quantum_usecs)
{
    _threadCount = 1;
    _qtime = 1;
    return 0;
}

int uthread_spawn(void (*f)(void))
{
    int id = GetNextId();
    if (id == -1)
    {
        return id;
    }

    _threads[id] = Thread(GetNextId(), f, STACK_SIZE);
    _runningTID = id;
    _roundRobinQueue.push_back(id);
    return id;
}

int uthread_terminate(int tid)
{
    if(_threads.find(tid) == _threads.end())
    {
        return -1;
    }

    _threads.erase(tid);
    _freeIds.push(tid);
    _roundRobinQueue.remove(tid);
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
