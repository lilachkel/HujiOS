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

Thread::Thread(int stackSize) : _id(0),
                                _stackSize(stackSize),
                                _quantums(1),
                                _isBlocked(false),
                                _isSynced(false),
                                _stack(new char[stackSize])
{}

Thread::Thread(int id, void (*job)(void), const int stackSize) :
        _id(id),
        _stackSize(stackSize),
        _quantums(0),
        _job(job),
        _isBlocked(false),
        _isSynced(false),
        _stack(new char[stackSize])
{
    address_t sp, pc;

    sp = (address_t) _stack + _stackSize - sizeof(address_t);
    pc = (address_t) _job;

    sigsetjmp(_env, BUF_VAL);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);
}

Thread::~Thread()
{
    delete[] _stack;
    _syncDeps.clear();
}

int Thread::SaveEnv()
{
    return sigsetjmp(_env, BUF_VAL);

}

void Thread::LoadEnv()
{
    siglongjmp(_env, BUF_VAL);
}

void Thread::AddSyncDep(int tid)
{
    if (!IsSyncedWith(tid))
        _syncDeps.push_back(tid);
}

bool Thread::IsSyncedWith(int tid)
{
    for (auto id : _syncDeps)
    {
        if (id == tid) return true;
    }

    return false;
}

const int Thread::Flags() const
{
    if (_isBlocked && !_isSynced) return BLOCKED;
    if (!_isBlocked && _isSynced) return SYNCED;
    if (_isBlocked && _isSynced) return BLOCKED_AND_SYNCED;
    return 0;
}
