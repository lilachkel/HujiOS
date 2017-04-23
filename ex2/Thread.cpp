//
// Created by jenia90 on 3/22/17.
//

#include <utility>
#include "Thread.h"

Thread::Thread(int id) : _id(id), _quantums(0), _isBlocked(false)
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
    //You get free! you get free!!! everybody gets free!!! (dont really know how.....)
    return 0;
}

/**
 *
 * @return :
 * 1 in case the thread was blocked (means we actually changed his status)
 * 0 in case the thread is in RUNNING or READY state.
 */
int Thread::Resume()
{
    if (_isBlocked)
    {
        _isBlocked = false;
        return 1;
    }
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



