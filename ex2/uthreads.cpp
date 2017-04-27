#include "uthreads.h"
#include "Thread.h"
#include <queue>
#include <map>
#include <set>
#include <sys/time.h>
#include <iostream>

using namespace std;

#define SYS_ERR "system error: "
#define LIB_ERR "thread library error: "

#define TID_SWITCH_ERR "can't switch to next thread!"
#define SELF_SYNC_ERR "cannot sync with itself!"
#define SYNCED_TID_ERR "Trying to sync synced thread!"
#define WRONG_TID_ERR "wrong TID!"
#define TID_NOT_FOUND_ERR "TID not found."
#define MAIN_BLOCK_ERR "cannot block main thread."
#define MAX_NUM_ERR "maximum number of threads reached."
#define SIG_ACT_ERR "unable to signal handler"
#define TIMER_SET_ERR "unable to set timer."
#define SAVE_ENV_ERR "unable to save thread stack."
#define SIGNAL_MASK_ERR "unable to mask signal."
#define SIGNAL_UNMASK_ERR "unable to unmask signal"
void PrintError(string kind, string msg);

#define SIGN_BLOCK if(sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL) == -1){\
    PrintError(SYS_ERR, SIGNAL_MASK_ERR);\
    exit(EXIT_FAILURE);}
#define SIGN_UNBLOCK if(sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL) == -1){\
    PrintError(SYS_ERR, SIGNAL_UNMASK_ERR);\
    exit(EXIT_FAILURE);}
#define BLOCK_CASE 'b'
#define SYNC_CASE 's'
#define TERMINATE_CASE 't'


int _threadCount, _runningTID, _qtime;
set<int> _freeIds;
list<int> _readyQueue;
map<int, Thread *> _threads;
struct sigaction sa;
struct itimerval timer;

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

    return nextTid;
}

/**
 * Prints an error message to the console.
 * @param msg the message string to print.
 */
void PrintError(string kind, string msg)
{
    cerr << kind << msg << endl;
}

/**
 * Iterates over a list of threads the current thread is synced with and resumes them
 * @param syncList ref to a list of synced IDs
 */
void ResumeSyncList(std::list<int> &syncList)
{
    for (auto id : syncList)
    {
        uthread_resume(id);
    }
    syncList.clear();
}

/**
 * Terminates all threads and frees the memory.
 */
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
    SIGN_BLOCK

    int nextThread = GetNextThread();

    ResumeSyncList(_threads[_runningTID]->GetSyncList());
    if (nextThread != -1)
    {
        _readyQueue.push_back(_runningTID);
        if(_threads[_runningTID]->SaveEnv())
        {
            PrintError(SYS_ERR, SAVE_ENV_ERR);
            SIGN_UNBLOCK
            exit(EXIT_SUCCESS);
        }
        _runningTID = nextThread;
        _threads[_runningTID]->IncrementQuanta();
        _threads[_runningTID]->LoadEnv();
    }
    else
    {
        _threads[_runningTID]->IncrementQuanta();
    }

    _qtime++;
    SIGN_UNBLOCK
}

/**
 * Terminates the given thread after realeasing all the resources allocated to it.
 * @param tid thread id
 */
void TerminateHelper(int tid)
{
    auto thread = _threads[tid];
    ResumeSyncList(thread->GetSyncList());
    _threads.erase(tid);
    delete thread;
    _readyQueue.remove(tid);
    _freeIds.insert(tid);
}

/**
 * Function which is called when a scheduling decision
 * should be made(running thread block itself, etc...).
 * @return On success, return 0. On failure, return -1.
 */
int runNext(char wantedCase)// i defined char since we use int too mach and maybe will get confused
{
    int nextThread = GetNextThread();

    switch (wantedCase)
    {
        case BLOCK_CASE:
            _threads[_runningTID]->SetBlock(true);
            _readyQueue.remove(_runningTID);
            if(_threads[_runningTID]->SaveEnv())
            {
                PrintError(SYS_ERR, SAVE_ENV_ERR);
                SIGN_UNBLOCK
                exit(EXIT_SUCCESS);
            }
            break;
        case SYNC_CASE:
            _threads[_runningTID]->SetSync(true);
            _readyQueue.remove(_runningTID);
            if(_threads[_runningTID]->SaveEnv())
            {
                PrintError(SYS_ERR, SAVE_ENV_ERR);
                SIGN_UNBLOCK
                exit(EXIT_SUCCESS);
            }
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
    ResumeSyncList(_threads[_runningTID]->GetSyncList());
    _threads[_runningTID]->IncrementQuanta();
    _threads[_runningTID]->LoadEnv();
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
    _qtime = 1;
    _threadCount = 1;

    timer.it_interval.tv_usec = quantum_usecs;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = quantum_usecs;

    sa.sa_handler = &timerHandler;
    if (sigaction(SIGVTALRM, &sa, NULL))
    {
        PrintError(SYS_ERR, SIG_ACT_ERR);
        exit(EXIT_FAILURE);
    }

    if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
    {
        PrintError(SYS_ERR, TIMER_SET_ERR);
        SIGN_UNBLOCK
        exit(EXIT_FAILURE);
    }

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
        PrintError(LIB_ERR, MAX_NUM_ERR);
        SIGN_UNBLOCK
        return -1;
    }

    _threads[id] = new Thread(id, f, STACK_SIZE);
    _readyQueue.push_back(id);
    SIGN_UNBLOCK

    return id;
}

int uthread_terminate(int tid)
{
    SIGN_BLOCK
    if (tid == 0)
    {
        TerminateAll();
        SIGN_UNBLOCK
        exit(EXIT_SUCCESS);
    }

    else if (_threads.find(tid) == _threads.end())
    {
        SIGN_UNBLOCK
        PrintError(LIB_ERR, TID_NOT_FOUND_ERR);
        return -1;
    }

    else if (_runningTID == tid)
    {// scheduling decision
        if(runNext('t') == -1)
        {
            SIGN_UNBLOCK
            PrintError(LIB_ERR, TID_SWITCH_ERR);
            return -1;
        }
    }

    TerminateHelper(tid);
    SIGN_UNBLOCK
    return 0;
}

int uthread_block(int tid)
{
    SIGN_BLOCK

    if (tid == 0)
    {
        PrintError(LIB_ERR, MAIN_BLOCK_ERR);
        SIGN_UNBLOCK
        return -1;
    }

    else if(_threads.find(tid) == _threads.end())
    {
        PrintError(LIB_ERR, TID_NOT_FOUND_ERR);
        SIGN_UNBLOCK
        return -1;
    }

    if (_runningTID == tid)
    {
        if (runNext('b') == -1)
        {
            PrintError(LIB_ERR, TID_SWITCH_ERR);
            SIGN_UNBLOCK
            return -1;
        }
    }

    _readyQueue.remove(tid);
    _threads[tid]->SetBlock(true);

    SIGN_UNBLOCK

    return 0;
}

int uthread_resume(int tid)
{
    SIGN_BLOCK

    if(_threads.find(tid) == _threads.end())
    {
        PrintError(LIB_ERR, WRONG_TID_ERR);
        SIGN_UNBLOCK
        return -1;
    }

    auto thread = _threads[tid];
    switch (thread->Flags())
    {
        case BLOCKED:
            thread->SetBlock(false);
            _readyQueue.push_back(tid);
            break;
        case SYNCED:
            thread->SetSync(false);
            _readyQueue.push_back(tid);
            break;
        case BLOCKED_AND_SYNCED:
            thread->SetBlock(false);
            break;
    }

    SIGN_UNBLOCK
    return 0;
}

int uthread_sync(int tid)
{
    if(_runningTID == 0 || _threads.find(tid) == _threads.end())
    {
        PrintError(LIB_ERR, WRONG_TID_ERR);
        SIGN_UNBLOCK
        return -1;
    }
    else if(_threads[_runningTID]->IsSynced())
    {
        PrintError(LIB_ERR, SYNCED_TID_ERR);
        SIGN_UNBLOCK
        return -1;
    }

    else if(_runningTID == tid)
    {
        PrintError(LIB_ERR, SELF_SYNC_ERR);
        SIGN_UNBLOCK
        return -1;
    }

    _threads[tid]->AddSyncDep(_runningTID);
    if(runNext(SYNC_CASE) == -1)
    {
        PrintError(LIB_ERR, TID_SWITCH_ERR);
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
    return _threads.find(tid) != _threads.end() ? _threads[tid]->GetQuantums() : -1;
}