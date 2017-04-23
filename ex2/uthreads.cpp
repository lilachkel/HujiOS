//
// Created by jenia90 on 3/22/17.
//

#include "uthreads.h"
#include "Thread.h"
#include <queue>
#include <map>
#include <list>
#include <set>
#include <sys/time.h>

#define SIGN_BLOCK if (sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL) == -1) \
{\
return -1;\
};
#define SIGN_UNBLOCK if (sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL) == -1) \
{\
return -1;\
};
#define BLOCK_CASE 'b'
#define TERMINATE_CASE 't'

using namespace std;

int _threadCount, _runningTID, _qtime;
set<int> _freeIds;
list<int> _readyQueue;
std::map<int,std::list<int>> _blockQueue;
map<int, Thread> _threads;
struct sigaction sa;
struct itimerval timer;

/**
 * Gets next thread ID in queue
 * @return next thread ID
 */
int GetNextThread()
{
    int nextTid = -1;

    if (!_readyQueue.empty())
    {
        nextTid = _readyQueue.front();
        _readyQueue.pop_front();
    }

    return nextTid;
}

/**
 * Function which is called upon SIGVTALRM
 * @param sig the signal which was called
 */
void timerHandler(int sig)
{
    if (sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL)==-1)
    {
        //ERROR NO RETURN VALUE
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

        //ERROR NO RETURN VALUE
    }


}
/**
 *
 * @param tid
 */
void TerminateHalper(int tid){

    int blockedTid;
    if (_blockQueue.find(tid) != _blockQueue.end()) {//todo: check if i did it right. im toooo tired
        for (int i = 0; i < _blockQueue[tid].size(); i++) {
            blockedTid = _blockQueue[tid].front();
            _blockQueue[tid].pop_front();
            uthread_resume(blockedTid);
        }
        _blockQueue.erase(tid);
    }
    _threads[tid].Terminate();
    _readyQueue.remove(tid);
    _threads.erase(tid);
    _freeIds.insert(tid);

}
/**
 * Function which is called when a scheduling decision
 * should be made(running thread block itself, ect...).
 * @return On success, return 0. On failure, return -1.
 */
int runNext(char wantedCase)// i defined char since we use int too mach and maybe will get confused
{
    // alredy at SIGN_UNBLOCK state.
     if(setitimer(ITIMER_VIRTUAL, &timer, NULL))//'rest' the timer
     {
         // print error ?
         return -1;
     }

     int nextThread = GetNextThread();
     if(nextThread == -1 ){// if there is no more threads in the ready list..?
        // call  thread 0? so should thread 0 be in the readyQ all times?...
     }
    switch(wantedCase) {
        case BLOCK_CASE:
            _threads[_runningTID].Block();
            _readyQueue.remove(_runningTID);
            break;
        case TERMINATE_CASE:
            TerminateHalper(_runningTID);
            break;
        default:
            SIGN_UNBLOCK
            return -1;
    }
     _runningTID = nextThread;
     _threads[_runningTID].LoadEnv();
     _qtime++;

    SIGN_UNBLOCK

    return 0;
 }

/**
 * Gets the first free id for a new thread.
 * @return int representing a new ID for a thread; -1 otherwise
 */
int GetNextFreeId()
{

    if(_freeIds.size() > 0)
    {
        int newTid = *_freeIds.begin();
        _freeIds.erase(newTid);
        return newTid; //need to make sure i did it right! 'set' keep their elements ordered at all times..
        // the begin sopposed to get the default iter which is ascending order, and... i returnd the pointer...
        // should be the wanted tid.....
    }

    return _threadCount < MAX_THREAD_NUM ? _threadCount++ : -1;
}

int uthread_init(int quantum_usecs)
{
    _threadCount = 1;
    timer.it_interval.tv_usec = quantum_usecs;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_usecs;

    sa.sa_handler = &timerHandler;
    if(sigaction(SIGVTALRM, &sa, NULL)) { return -1; }

    if(setitimer(ITIMER_VIRTUAL, &timer, NULL)) { return -1; }

    _threads[0] = Thread(0);
    _qtime = 1; //since thread 0 started
    return 0;
}
/**
 * This function creates a new thread.
 * @param f The function which the thread need to perform.
 * @return On success, return the ID of the created thread.
 * On failure, return -1.
 */
int uthread_spawn(void (*f)(void))
{
    SIGN_BLOCK
    int id = GetNextFreeId();
    if (id == -1)
    {
        return -1;
    }

    _threads[id] = Thread(id, f, STACK_SIZE);
    if(_readyQueue.empty()) _runningTID = id;
    _readyQueue.push_back(id);
    _threadCount++;

    SIGN_UNBLOCK

    return id;
}

int uthread_terminate(int tid)// free BLOCKED threads(+change there state), delete stack, save ID/TID in '_freeIds'
// the tid == 0 should be in the READY list?  since we should be the one to terminate tid==0 while we terminate the whole program...
// if not so how can we tell call 'exit(0) ..? if we do- we dont know what written
{
    SIGN_BLOCK

    if(tid == 0 || _threads.find(tid) == _threads.end())
    {// trying to block the first thread or there is no such thread
        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
        return -1;
    }
    if (_runningTID == tid){// scheduling decision
        if(runNext('t') < 0)
        {
            sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
            return -1; // or end the process.
        }
    }
    TerminateHalper(tid);

    SIGN_UNBLOCK
    return 0;
}

/**
 *
 * @param tid
 * @return
 */
int uthread_block(int tid)
{
    SIGN_BLOCK

    if(tid == 0 || _threads.find(tid) == _threads.end())
    {
        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
        return -1;
    }

    if (_runningTID == tid)
    {
        if(runNext('b') < 0)
        {
            sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
            return -1;
        }
    }
    SIGN_UNBLOCK

    return _threads[tid].Block();
}

int uthread_resume(int tid)
{//since we have to push the unblocked thread to the ready list again...
    SIGN_BLOCK
    if(_threads.find(tid) == _threads.end())
    {
        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
        return  -1 ;
    }
    if(_threads[tid].Resume()) // in case the thread was blocked, and his tid is not in readyQ
    {
        _readyQueue.push_back(tid);
    }
    SIGN_UNBLOCK
    return 0;
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
    return _threads.find(tid) == _threads.end() ? -1 : _threads[tid].GetQuantums();
}