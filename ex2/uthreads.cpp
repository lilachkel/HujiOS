//
// Created by jenia90 on 3/22/17.
//

#include "uthreads.h"
#include <queue>
using namespace std;

class Thread
{
private:
public:

};

class Lin

//typedef struct Thread *ThreadP;
//struct Thread
//{
//    int tid;
//    void (*job)(void);
//    ThreadP _next;
//};
//
//typedef struct LinkedList
//{
//    ThreadP _head;
//    ThreadP _tail;
//} *LLP;

queue<int> freeIds;

int uthread_init(int quantum_usecs)
{
    return 0;
}

int uthread_spawn(void (*f)(void))
{

}

int uthread_terminate(int tid)
{
    return 0;
}

int uthread_block(int tid)
{
    return 0;
}

int uthread_resume(int tid)
{
    return 0;
}

int uthread_sync(int tid)
{
    return 0;
}

int uthread_get_tid()
{
    return 0;
}

int uthread_get_total_quantums()
{
    return 0;
}
