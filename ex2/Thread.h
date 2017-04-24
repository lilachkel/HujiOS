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

#define BUF_VAL 1



class Thread
{
private:
    int _id, _stackSize, _quantums;
    void (*_job)(void);
    bool _isBlocked;
    sigjmp_buf _env;
    char *_stack;

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

    /**
     * Gets block status of this Thread
     * @return
     */
    const bool GetBlockStatus() const;

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
    inline const int IncrementQuanta() { _quantums++; }

    /**
     * Current quanta count
     * @return
     */
    int GetQuantums();

    /**
     * Blocks the thread
     * @return 0 if successful; -1 otherwise
     */
    int Block();

    /**
     * Unblocks the thread
     * @return 0 if successful; -1 otherwise
     */
    int Resume();
};


#endif //PROJECT_THREAD_H
