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
    _qtime = 1; //not the input(quantum_usecs), or is that the counter ?
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

int uthread_terminate(int tid)// free BLOCKED threads(+change there state), delete stack, save ID/TID in '_freeIds'
// the tid == 0 should be in the READY list?  since we should be the one to terminate tid==0 while we terminate the whole program...
// if not so how can we tell call 'exit(0) ..? if we do- we dont know what written in tid==0 so how we avoid bugs?
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

int uthread_block(int tid)// we(the schedule) need to make sure the tid that block itself is the one to release itself+

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
