#ifndef PROJECT_THREAD_H
#define PROJECT_THREAD_H

#include <setjmp.h>
#include <signal.h>
#include <utility>
#include <list>
#include <algorithm>

#define BUF_VAL 1

#define BLOCKED 1
#define SYNCED 2
#define BLOCKED_AND_SYNCED 3

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

    /**
     * Gets the sync-block flag of this thread.
     * @return 1 for blocked but not synced; 2 for synced but not blocked; 3 for synced and blocked.
     */
    const int Flags() const;

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
     */
    inline int GetQuantums() const { return _quantums; }

    /**
     * Sets the state of the block flag
     */
    inline void SetBlock(bool block) { _isBlocked = block; }

    /**
     * Returns the sync flag status
     * @return true if synced; false otherwise.
     */
    inline bool IsSynced() const { return _isSynced; }

    /**
     * Sets sync flag state
     */
    inline void SetSync(bool sync) { _isSynced = sync; }

    /**
     * Adds a TID to a list of threads that block this thread.
     * @param tid the id of a thread that blocks this thread.
     */
    void AddSyncDep(int tid);

    /**
     * Gets the list of TID that are synced with this thread..
     * @return ref to the list container.
     */
    inline std::list<int>&GetSyncList() { return _syncDeps; }
};

#endif //PROJECT_THREAD_H
