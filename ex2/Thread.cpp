//
// Created by jenia90 on 3/22/17.
//

#include "Thread.h"

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
            "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

Thread::Thread() : _id(0), _quantums(1), _isBlocked(false)
{
    Setup();
}

Thread::Thread(int id, void (*job)(void), const int stackSize) :
        _id(id),
        _job(job),
        _isBlocked(false),
        _stackSize(stackSize)
{
    Setup();
}

Thread::~Thread()
{

}


const int Thread::GetId() const
{
    return _id;
}

int Thread::SaveEnv()
{
    if (sigsetjmp(_env, BUF_VAL) == BUF_VAL)
    {
        return 0;
    }
    return -1;

}

void Thread::LoadEnv()
{
    siglongjmp(_env, BUF_VAL);
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

    sp = (address_t) _stack + _stackSize - sizeof(address_t);
    pc = (address_t) _job;

    sigsetjmp(_env, BUF_VAL);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);
}




