//
// Created by jenia90 on 3/22/17.
//

#ifndef PROJECT_THREAD_H
#define PROJECT_THREAD_H


//#define RUNNING = 1
//#define BLOCKED = 2
//#define READY = 3

#include "stdlib.h"


class Thread
{
private:
    int _id, _stackSize;
    void (*_job)(void);
    bool _isBlocked;
    int _
    int _quantumsNum; //for 'uthread_get_quantums' functions

//    int _state; //RUNNING, BLOCKED, or READY instead of the  isbloced+isrunning+is ready..?

public:
    Thread(Thread && thread);
    Thread(const int id, const void (*job)(void), const int stackSize);
    ~Thread();

    /**
     * Gets the TID of this thread
     * @return
     */
    int GetId() const;
    bool GetBlockStatus() const;
    void SetBlockStatus(const bool isBlocked);

    int Execute();
    int Block();
    int Terminate();
    int Resume();
    int Sync();
};


#endif //PROJECT_THREAD_H
