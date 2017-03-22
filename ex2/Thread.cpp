//
// Created by jenia90 on 3/22/17.
//

#include "Thread.h"

Thread::Thread(int id, void (*job)(void), Thread next) : _id(id),
                                                         _job(job),
                                                         _next(next),
                                                         _isBlocked(false)
{}


int Thread::GetId()
{
    return _id;
}

Thread Thread::GetNext()
{
    return _next;
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

