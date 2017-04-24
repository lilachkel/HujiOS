//#define DEBUG

#include "uthreads.h"
#include "Thread.h"
#include <queue>
#include <map>
#include <list>
#include <set>
#include <sys/time.h>
#include <iostream>

#define SIGN_BLOCK if (sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL) == -1) { return -1; }
#define SIGN_UNBLOCK if (sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL) == -1) { return -1; }
#define BLOCK_CASE 'b'
#define TERMINATE_CASE 't'

using namespace std;

int _threadCount, _runningTID, _qtime;
set<int> _freeIds;
list<int> _readyQueue;
map<int, std::list<int>> _blockQueue;
map<int, Thread*> _threads;
struct sigaction sa;
struct itimerval timer;

#ifdef DEBUG
void PrintThreadInfo(string op)
{
    cout << "method: " << op << " | ready queue: ";
    for(int id: _readyQueue)
        cout  << id << " ";
    cout << "\n" << endl;
}
#endif

/**
 * Gets next thread ID in queue
 * @return next thread ID
 */
int GetNextThread()
{
    int nextTid = -1;

    if (_readyQueue.size() > 0)
    {
        nextTid = _readyQueue.front();
        _readyQueue.pop_front();
    }

#ifdef DEBUG
    PrintThreadInfo("GetNextId: " + to_string(nextTid));
#endif

    return nextTid;
}

/**
 * Function which is called upon SIGVTALRM
 * @param sig the signal which was called
 */
void timerHandler(int sig)
{
    sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);

    int nextThread = GetNextThread();

#ifdef DEBUG
    cout << "running thread: " << _runningTID << " | next thread: " << nextThread << endl;
#endif

    if (nextThread != -1)
    {
        _readyQueue.push_back(_runningTID);
        _threads[_runningTID]->SaveEnv();
        _runningTID = nextThread;
        _threads[_runningTID]->IncrementQuanta();
        _qtime++;
        _threads[_runningTID]->LoadEnv();
    }
    else
    {
        _threads[_runningTID]->IncrementQuanta();
        _qtime++;
    }

#ifdef DEBUG
    PrintThreadInfo("timer");
#endif

    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
}

/**
 *
 * @param tid
 */
void TerminateHelper(int tid)
{
    int blockedTid;
    if (_blockQueue.find(tid) != _blockQueue.end())
    {//todo: check if i did it right. im toooo tired
        for (int id : _blockQueue[tid])
        {
            blockedTid = _blockQueue[tid].front();
            _blockQueue[tid].pop_front();
            uthread_resume(blockedTid);
        }
        _blockQueue.erase(tid);
    }

    // TODO: check if the following shit is valid code!
    _threads.erase(tid);
    delete _threads[tid];
    _readyQueue.remove(tid);
    _freeIds.insert(tid);

#ifdef DEBUG
    PrintThreadInfo("Terminator");
#endif
}

/**
 * Function which is called when a scheduling decision
 * should be made(running thread block itself, ect...).
 * @return On success, return 0. On failure, return -1.
 */
int runNext(char wantedCase)// i defined char since we use int too mach and maybe will get confused
{
    int nextThread = GetNextThread();
    if (nextThread == -1) { return -1; }
    switch (wantedCase)
    {
        case BLOCK_CASE:
            _threads[_runningTID]->Block();
            _readyQueue.remove(_runningTID);
            _threads[_runningTID]->SaveEnv();
            break;
        case TERMINATE_CASE:
            TerminateHelper(_runningTID);
            break;
        default:
            SIGN_UNBLOCK
            return -1;
    }

    _runningTID = nextThread;
    _threads[_runningTID]->LoadEnv();
    _threads[_runningTID]->IncrementQuanta();
    _qtime++;
#ifdef DEBUG
    PrintThreadInfo("RunNext");
#endif
    SIGN_UNBLOCK

    return 0;
}

/**
 * Gets the first free id for a new thread.
 * @return int representing a new ID for a thread; -1 otherwise
 */
int GetNextFreeId()
{
    if (_freeIds.size() > 0)
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
    _threads[0] = new Thread(STACK_SIZE);
    _qtime = 1; //since thread 0 started
    _threadCount = 1;

    timer.it_interval.tv_usec = quantum_usecs;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_usecs;

    sa.sa_handler = &timerHandler;
    if (sigaction(SIGVTALRM, &sa, NULL))
    { return -1; }

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
    { return -1; }
#ifdef DEBUG
    PrintThreadInfo("Init");
#endif
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

    _threads[id] = new Thread(id, f, STACK_SIZE);
    _readyQueue.push_back(id);
#ifdef DEBUG
    PrintThreadInfo("Spawn");
#endif
    SIGN_UNBLOCK

    return id;
}

int uthread_terminate(int tid)// free BLOCKED threads(+change there state), delete stack, save ID/TID in '_freeIds'
// the tid == 0 should be in the READY list?  since we should be the one to terminate tid==0 while we terminate the whole program...
// if not so how can we tell call 'exit(0) ..? if we do- we dont know what written
{
    SIGN_BLOCK

    if (tid == 0 || _threads.find(tid) == _threads.end())
    {// trying to block the first thread or there is no such thread
        if(tid == 0 && _readyQueue.size() == 0)
        {
            TerminateHelper(tid);
            SIGN_UNBLOCK
            exit(EXIT_SUCCESS);
        }

        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
        return -1;
    }
     else if (_runningTID == tid)
    {// scheduling decision
        if (runNext('t') == -1)
        {
            sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
            return -1; // or end the process.
        }
    }
    TerminateHelper(tid);
#ifdef DEBUG
    PrintThreadInfo("Terminate");
#endif
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

    if (tid == 0 || _threads.find(tid) == _threads.end())
    {
        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
        return -1;
    }

    if (_runningTID == tid)
    {
        if (runNext('b') == -1)
        {
            sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
            return -1;
        }
    }
    SIGN_UNBLOCK

    return _threads[tid]->Block();
}

int uthread_resume(int tid)
{//since we have to push the unblocked thread to the ready list again...
    SIGN_BLOCK
    if (_threads.find(tid) == _threads.end())
    {
        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
        return -1;
    }
    if (_threads[tid]->Resume()) // in case the thread was blocked, and his tid is not in readyQ
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
    return _threads.find(tid) == _threads.end() ? -1 : _threads[tid]->GetQuantums();
}