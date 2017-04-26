//#define DEBUG

#include "uthreads.h"
#include "Thread.h"
#include <queue>
#include <map>
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
map<int, Thread *> _threads;
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
 * Prints an error message to the console.
 * @param msg the message string to print.
 */
void PrintError(bool isSysCall, string msg)
{
    if(isSysCall)
        cout << "system error: " << msg << endl;
    else
        cout << "thread library error: " << msg << endl;
}

void ResumeBlockList(std::list<int>& blockList)
{
    for (auto id : blockList)
    {
        uthread_resume(id);
    }
}

void TerminateAll()
{
    for(auto &t : _threads)
    {
        delete t.second;
    }

    _threads.clear();
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
        _threads[_runningTID]->LoadEnv();
    }
    else
    {
        _threads[_runningTID]->IncrementQuanta();
    }

#ifdef DEBUG
    PrintThreadInfo("timer");
#endif

    _qtime++;
    // not sure if Jenia did it right but, lets hope for the best.
    ResumeBlockList(_threads[_runningTID]->GetBlockList());
    sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
}

/**
 *
 * @param tid
 */
void TerminateHelper(int tid)
{
    auto thread = _threads[tid];
    ResumeBlockList(thread->GetBlockList());
    _threads.erase(tid);
    delete thread;
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
    if (nextThread == -1)
    { return -1; }
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
    _qtime++;
    _threads[_runningTID]->IncrementQuanta();
    _threads[_runningTID]->LoadEnv();
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
        return newTid;
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
        if (tid == 0)
        {
            TerminateAll();
            SIGN_UNBLOCK
            exit(EXIT_SUCCESS);
        }

        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
        return -1;
    }

    else if (_runningTID == tid)
    {// scheduling decision
        int ret_val = runNext('t');
        SIGN_UNBLOCK
        return ret_val;
    }

    TerminateHelper(tid);
#ifdef DEBUG
    PrintThreadInfo("Terminate");
#endif
    SIGN_UNBLOCK
    return 0;
}

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

    _threads[tid]->SaveEnv();
    _threads[tid]->Block();
    return 0;
}

int uthread_resume(int tid)
{//since we have to push the unblocked thread to the ready list again...
    SIGN_BLOCK
    if (tid == 0 || _threads.find(tid) == _threads.end())
    {
        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
        return -1;
    }

    if (_threads[tid]->IsBlocked()) // in case the thread was blocked, and his tid is not in readyQ
    {
        _threads[tid]->Resume();
        _readyQueue.push_back(tid);
    }
    SIGN_UNBLOCK
    return 0;
}

int uthread_sync(int tid)
{
    SIGN_BLOCK
    if(_runningTID == 0 || _threads.find(tid) == _threads.end())
    {
#ifdef DEBUG
        PrintThreadInfo("Sync");
#endif
        SIGN_UNBLOCK
        PrintError(false, "wrong TID!");
        return -1;
    }

    else if(_runningTID == tid)
    {
        SIGN_UNBLOCK
        PrintError(false, "cannot sync with itself!");
        return -1;
    }

    _threads[tid]->AddBlockDep(_runningTID);
    if(runNext('b') == -1)
    {
        SIGN_UNBLOCK
        return -1;
    }

    SIGN_UNBLOCK
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