//
// Created by jenia90 on 3/22/17.
//

#ifndef PROJECT_THREAD_H
#define PROJECT_THREAD_H

#include "stdlib.h"


class Thread
{
private:
    int _id, _stackSize;
    void (*_job)(void);
    bool _isBlocked;

public:
    Thread(int id, void (*job)(void), int stackSize);

    /**
     * Gets the TID of this thread
     * @return
     */
    int GetId();
    bool GetBlockStatus();
    void SetBlockStatus(bool isBlocked);

    int Execute();
    int Block();
    int Terminate();
    int Resume();
    int Sync();
};


#endif //PROJECT_THREAD_H
