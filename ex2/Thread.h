//
// Created by jenia90 on 3/22/17.
//

#ifndef PROJECT_THREAD_H
#define PROJECT_THREAD_H


class Thread
{
private:
    int _id;
    void (*_job)(void);
    Thread _next;
    bool _isBlocked;

public:
    Thread(int id, void (*job)(void), Thread next);

    /**
     * Gets the TID of this thread
     * @return
     */
    int GetId();
    Thread GetNext();
    bool GetBlockStatus();
    void SetBlockStatus(bool isBlocked);

    int Execute();
    int Block();
    int Terminate();
    int Resume();
    int Sync();
};


#endif //PROJECT_THREAD_H
