//
// Created by jenia90 on 4/30/17.
//

#include "MapReduceFramework.h"


pthread_mutex_t pthreadToContainer_mutex; //
sem_t ShuffleSemaphore;
std::vector<pthread_t> ExecMap;
std::vector<pthread_t> ExecReduce;
pthread_t shuffleThread;

std::map pthreadToContainer<pthread_t ,MAP_CONTAINER>;//container of <K2,V2> after the ExecMap job


void* ExecMapJob(void* mapReduce)
{
    pthread_mutex_lock(&pthreadToContainer_mutex);
    pthread_mutex_unlock(&pthreadToContainer_mutex);
    while(true)
    {}



}
void* ExecReduceJob(void* mapReduce)
{


}

void* ExecShuffle(void* mapReduce) {
    while (true) {
        sem_wait(&ShuffleSemaphore);
        pthread_t curThreadID = pthread_self();
        pthreadToContainer.at(curThreadID);
        pthreadToContainer.at(curThreadID).~vector;// to erase the thread cur container
        sem_post(&ShuffleSemaphore);
        if()
        {}

    }
}
OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase& mapReduce, IN_ITEMS_VEC& itemsVec,
                                    int multiThreadLevel, bool autoDeleteV2K2)
{

    sem_init(&ShuffleSemaphore, 0 ,0);// the first 0 is correct?
    pthread_mutex_init(&pthreadToContainer_mutex, NULL);

    pthread_mutex_lock(&pthreadToContainer_mutex);
    // to create the map threads which starts with 2 lines of lock and unlock pthreadToContainer_mutex
    for(int i = 0 ; i< multiThreadLevel; i++)//if itemsVec size is <10
    {
        if(pthread_create(&ExecMap[i], NULL, ExecMapJob,  &mapReduce)!= 0 )
        {
//            print an error message : "MapReduceFramework Failure: FUNCTION_NAME failed.", where FUNCTION_NAME is the
//            name of the library call that was failed [e.g. "new"].
            pthread_mutex_destroy(&pthreadToContainer_mutex);
            exit(1);
        }

    }
    pthread_create(&shuffleThread, NULL, ExecShuffle,  NULL );



    for(int i = 0 ; i< multiThreadLevel; i++)//after the map work
    {
        pthread_create(&ExecReduce[i], NULL, ExecReduceJob,  &mapReduce);

    }




    pthread_mutex_destroy(&pthreadToContainer_mutex);

}

void Emit2 (k2Base* k2, v2Base* v2)
{

    pthreadToContainer[pthread_self()].insert(k2, v2);



}
void Emit3 (k3Base*, v3Base*)
{}
