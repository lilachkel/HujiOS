//
// Created by jenia90 on 3/22/17.
//

#include <utility>
#include "Thread.h"

Thread::Thread(Thread &&thread) : _id(std::move(thread._id)),
                                  _job(std::move(thread._job)),
                                  _stackSize(std::move(thread._stackSize)),
                                  _isBlocked(thread._isBlocked)
{}

Thread::Thread(const int id, const void (*job)(void), const int stackSize) :
        _id(id),
        _job(job),
        _isBlocked(false),
        _stackSize(stackSize)
{}

Thread::~Thread()
{
    Terminate();
    delete _job;
}


int Thread::GetId() const
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

int Thread::Pause()
{
    return 0;
}

int Thread::Block()
{
    return 0;
}

bool Thread::GetBlockStatus() const
{
    return _isBlocked;
}

void Thread::SetBlockStatus(const bool isBlocked)
{
    _isBlocked = isBlocked;
}



