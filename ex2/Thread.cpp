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
{
    Setup();
}

Thread::~Thread()
{
    Terminate();
    delete _stack;
}


const int Thread::GetId() const
{
    return _id;
}

//int Thread::Execute()
//{
//
//    return 0;
//}
void Thread::SaveEnv()
{
    sigsetjmp(_env, BUF_VAL);
}
void Thread::LoadEnv()
{
    _quantums++;
    siglongjmp(_env, BUF_VAL);
}

int Thread::Terminate()
{
    return 0;
}

int Thread::Resume()
{
    return 0;
}

int Thread::Block()
{
    _isBlocked = true;
    SaveEnv();//to block here too?


    return 0;
}

const bool Thread::GetBlockStatus() const
{
    return _isBlocked;
}

int Thread::GetQuantums()
{
    return _quantums;
}

void Thread::SetBlockStatus(const bool isBlocked)
{
    _isBlocked = isBlocked;
}

void Thread::Setup()
{
    address_t sp, pc;
    _stack = new char[_stackSize];

    sp = (address_t)_stack + _stackSize - sizeof(address_t);
    pc = (address_t)_job;

    sigsetjmp(_env, BUF_VAL);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);
}



