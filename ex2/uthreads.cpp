//
// Created by jenia90 on 3/22/17.
//

#include "uthreads.h"
#include "Thread.h"
#include <queue>
#include <map>
#include <list>
#include <sys/time.h>
#include <signal.h>

using namespace std;

int _threadCount, _runningTID, _qtime;
queue<int> _freeIds;
list<int> _readyQueue, _blockQueue;
map<int, Thread> _threads;

void timerHandler(int sig)
{

}

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
    struct sigaction sa;
    struct itimerval timer;

    _threadCount = 1;
    timer.it_interval.tv_usec = quantum_usecs;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_usecs;

    sa.sa_handler = &timerHandler;
    if(sigaction(SIGVTALRM, &sa, NULL)) { return -1; }
    if(setitimer(ITIMER_VIRTUAL, &timer, NULL)) { return -1; }

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
    _readyQueue.push_back(id);
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
    _readyQueue.remove(tid);
    return 0;
}

int uthread_block(int tid)// we(the schedule) need to make sure the tid that block itself is the one to release itself+

{
    return _threads[tid].Block();
}

int uthread_resume(int tid)
{
    return _threads[tid].Resume();
}

int uthread_sync(int tid)
{
    return 0;
}

int uthread_get_tid()
{
    return _runningTID;
}

int uthread_get_total_quantums()
{
    return _qtime;
}
