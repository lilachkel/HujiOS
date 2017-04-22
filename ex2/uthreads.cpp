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

int _threadCount, _runningTID, _qtime, _currentIndex;
queue<int> _freeIds;
list<int> _readyQueue, _blockQueue;
map<int, Thread> _threads;
struct sigaction sa;
struct itimerval timer;

int GetNextThread()
{
    if (!_readyQueue.empty())
    {
        return ++_currentIndex;
    }

    return -1;
}

void timerHandler(int sig)
{
    if (sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL)==-1)
    {
        //ERROR
    }

    if(setitimer(ITIMER_VIRTUAL, &timer, NULL))
    { //'rest' the timer
        // print error &something
    }

    int nextThread = GetNextThread();
    if(nextThread == -1 ){// if there is no more threads in the ready list...?

    }

    _readyQueue.push_back(_runningTID);
    _threads[_runningTID].SaveEnv();
    _runningTID = nextThread;
    _threads[_runningTID].LoadEnv();
    _qtime++;
    if (sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL)==-1)
    {
        //ERROR
    }
}

int runNext(){// for block case
     if (sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL)==-1)
     {
         //ERROR
         return -1;// every time i check it??
     }

     if(setitimer(ITIMER_VIRTUAL, &timer, NULL))
     { //'rest' the timer
         // print error &something
         return -1;// every time i check it??
     }

     int nextThread = GetNextThread();
     if(nextThread == -1 ){// if there is no more threads in the ready list..?

     }

     _threads[_runningTID].Block();
     _blockQueue.push_back(_runningTID);
     _threads.erase(_runningTID);
     _runningTID = nextThread;
     _threads[_runningTID].LoadEnv();
     _qtime++;

     if (sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL)==-1)
     {
         //ERROR
         return -1;// every time i check it??
     }

    return 0;
 }


int GetNextFreeId()
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
    // TODO: Add handler for TID 0.

    _currentIndex = 0;
    _threadCount = 1;
    timer.it_interval.tv_usec = quantum_usecs;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_usecs;

    sa.sa_handler = &timerHandler;
    if(sigaction(SIGVTALRM, &sa, NULL)) { return -1; }
    if(setitimer(ITIMER_VIRTUAL, &timer, NULL)) { return -1; }

    _qtime = 1; //since thread 0 started
    return 0;
}

int uthread_spawn(void (*f)(void))
{
    int id = GetNextFreeId();
    if (id == -1)
    {
        return -1;
    }

    if (sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL) == -1)
    {
        return -1;
    }

    _threads[id] = Thread(GetNextFreeId(), f, STACK_SIZE);
    if(_readyQueue.empty()) _runningTID = id;
    _readyQueue.push_back(id);
    _threadCount++;

    if (sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL) == -1)
    {
        return -1;
    }

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

int uthread_block(int tid)// we(the schedule) need to make sure the tid that block itself is not the one to release itself+

{
    if(tid == 0 || _threads.find(tid) == _threads.end())
    {// trying to block the first thread or there is no such thread
        //ERROR
        return -1;
    }

    if (_runningTID == tid)
    {
        runNext();
    }

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
int uthread_get_quantums(int tid)
{
    return _threads[tid].GetQuantums();
}