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
#include "uthreads.h"

#define BUF_VAL 1



class Thread
{
private:
    int _id, _stackSize, _quantums;
    void (*_job)(void);
    bool _isBlocked;
    sigjmp_buf _env;
    char _stack[STACK_SIZE];

    void Setup();

//    int _state; //RUNNING, BLOCKED, or READY instead of the  isbloced+isrunning+is ready..?

public:
    Thread();
    Thread(int id, void (*job)(void), const int stackSize);
    ~Thread();

    /**
     * Gets the TID of this thread
     * @return
     */
    const int GetId() const;
    const bool GetBlockStatus() const;
    void SetBlockStatus(const bool isBlocked);
    int SaveEnv();
    void LoadEnv();

    inline const int IncrementQuanta() { _quantums++; }
    int GetQuantums();
    int Block();
    int Resume();
};


#endif //PROJECT_THREAD_H
