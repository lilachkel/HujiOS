//
// Created by jenia90 on 3/22/17.
//

#include "Thread.h"

Thread::Thread(int id, void (*job)(void), int stackSize) : _id(id),
                                                           _job(job),
                                                           _isBlocked(false),
                                                           _stackSize(stackSize)
{}


int Thread::GetId()
{
    return _id;
}

int Thread::Execute()
{
    _job();
    return 0;
}

int Thread::Terminate()
{
    return 0;
}

int Thread::Resume()
{
    return 0;
}

int Thread::Sync()
{
    return 0;
}

int Thread::Block()
{
    return 0;
}

bool Thread::GetBlockStatus()
{
    return _isBlocked;
}

void Thread::SetBlockStatus(bool isBlocked)
{
    _isBlocked = isBlocked;
}

