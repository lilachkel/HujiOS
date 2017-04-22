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

#define BUF_VAL 1

#ifdef __x86_64__
/* code for 64 bit Intel arch */

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

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif

class Thread
{
private:
    int _id, _stackSize, _quantums;
    void (*_job)(void);
    bool _isBlocked;
    sigjmp_buf _env;
    char *_stack;

    void Setup();

//    int _state; //RUNNING, BLOCKED, or READY instead of the  isbloced+isrunning+is ready..?

public:
    Thread(Thread && thread);
    Thread(const int id, const void (*job)(void), const int stackSize);
    ~Thread();

    /**
     * Gets the TID of this thread
     * @return
     */
    const int GetId() const;
    const bool GetBlockStatus() const;
    void SetBlockStatus(const bool isBlocked);
    void SaveEnv();
    void LoadEnv();

    int GetQuantums();
    int Block();
    int Terminate();
    int Resume();
};


#endif //PROJECT_THREAD_H
