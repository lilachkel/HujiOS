//
// Created by jenia90 on 3/22/17.
//

#ifndef PROJECT_THREAD_H
#define PROJECT_THREAD_H


//#define RUNNING = 1
//#define BLOCKED = 2
//#define READY = 3

#include <setjmp.h>
#include <signal.h>
#include <utility>
#include <list>
#include <algorithm>

#define BUF_VAL 1



class Thread
{
private:
    int _id, _stackSize, _quantums;
    void (*_job)(void);
    bool _isBlocked, _isSynced;
    sigjmp_buf _env;
    char *_stack;
    std::list<int> _syncDeps;

    /**
     * Checks if a specific thread blocks this thread.
     * @param tid the thread ID in question.
     * @return true if blocks; false otherwise
     */
    bool IsSyncedWith(int tid);
//    int _state; //RUNNING, BLOCKED, or READY instead of the  isbloced+isrunning+is ready..?

public:
    /**
     * Jobless ctor.
     * Warning: Should be used for main thread only.
     * @param stackSize Thread stack size
     */
    Thread(int stackSize);

    /**
     * Thread ctor
     * @param id Thread ID
     * @param job function pointer which will be executed by this thread
     * @param stackSize Thread stack size
     */
    Thread(int id, void (*job)(void), const int stackSize);

    /**
     * dtor.
     */
    ~Thread();

    /**
     * Gets the TID of this thread
     */
    const int GetId() const;

    const int Flags()
    {
        if(_isBlocked && !_isSynced) return 1;
        if(!_isBlocked && _isSynced) return 2;
        if(_isBlocked && _isSynced) return 3;
    }

    /**
     * Gets block status of this Thread
     * @return
     */
    inline bool IsBlocked() const { return _isBlocked; }

    /**
     * Saves the stack environment for this thread
     * @return -1 if unsuccessful; 0 otherwise
     */
    int SaveEnv();

    /**
     * Restores the stack environment for this thread
     */
    void LoadEnv();

    /**
     * Increments the quanta counter
     * @return current quanta count
     */
    inline const void IncrementQuanta() { _quantums++; }

    /**
     * Current quanta count
     * @return
     */
    inline int GetQuantums() const { return _quantums; }

    /**
     * Blocks the thread
     * @return 0 if successful; -1 otherwise
     */
    inline void SetBlock(bool block) { _isBlocked = block; }

    inline bool IsSynced() const { return _isSynced; }

    inline void SetSync(bool sync) { _isSynced = sync; }

    /**
     * Adds a TID to a list of threads that block this thread.
     * @param tid the id of a thread that blocks this thread.
     */
    void AddSyncDep(int tid);

    /**
     * Removes a TID from a list of threads that block this thread.
     * @param tid the id of a thread that blocks this thread.
     */
    void RemoveSyncDep(int tid);

    inline std::list<int>& GetSyncList() { return _syncDeps; }
};


#endif //PROJECT_THREAD_H
