
#include "MapReduceFramework.h"

IN_ITEMS_VEC _itemsVec;
MapReduceBase* _mapReduce;
std::vector<pthread_t> ExecMap;
//std::vector<pthread_t> ExecReduce;
std::map<pthread_t ,MAP_CONTAINER> pthreadToContainer;//container of <K2,V2> after the ExecMap job
std::map<k2Base*, V2_VEC> shuffledList;
std::vector<SHUFFLED_ITEM> Shuffle_vec;
std::map<pthread_t,pthread_mutex_t> MapContainer_mutex;

int popIndex;
bool StupidVar;
pthread_mutex_t pthreadToContainer_mutex;
pthread_mutex_t popIndex_mutex;
pthread_t shuffleThread;
sem_t ShuffleSemaphore;



void* ExecMapJob(void* mapReduce)
{
    pthread_mutex_lock(&pthreadToContainer_mutex);
    pthread_mutex_unlock(&pthreadToContainer_mutex);
    int chunk_ind;

    while(true)
    {
        int i;
        pthread_mutex_lock(&popIndex_mutex);
        if(popIndex == -1)
        {
            pthread_mutex_unlock(&popIndex_mutex);
            return nullptr;
        }
        chunk_ind = popIndex;
        if (popIndex-10>0)
        {
            popIndex -= 10;
            pthread_mutex_unlock(&popIndex_mutex);
            i = 10;
        }else
        {
            i = chunk_ind - popIndex;
            popIndex = -1;
            pthread_mutex_unlock(&popIndex_mutex);// did it twice and not
            // under the ifelse since i wanted to unlock the mutex before i change 'i' to -1,
            // so that other threads will continue
        }
        for (;i>=0;i--)
        {
            _mapReduce->Map(_itemsVec[chunk_ind].first, _itemsVec[chunk_ind].second);
            chunk_ind -=1;
        }
    }



}
void* ExecReduceJob(void* mapReduce)
{


}

void* ExecShuffle(void* mapReduce) {

    int i = 0;
    while (true)
    {
        sem_wait(&ShuffleSemaphore);
        sem_getvalue(&ShuffleSemaphore, &i);
        if(StupidVar && i==0)
        {
            for (std::map<k2Base*, V2_VEC>::const_iterator _key = shuffledList.cbegin(); _key != shuffledList.cend() ; _key++)
            {
                Shuffle_vec.push_back(std::make_pair(_key->first,_key->second));// pushes a new pair?
                // delete such new objects i created?
                shuffledList.erase(_key);// check the iterator support the deletion
            }



            return nullptr;
        }
        for (std::map<pthread_t ,MAP_CONTAINER>::iterator it=pthreadToContainer.begin(); it!=pthreadToContainer.end(); ++it)
        {
            if (it->second.size()<=0)// not lock by mutex since the case which it gets the 'wrong' results-
                // the 'post' call called by other thread -  check if true
            {
                continue;//search the first not-empty container and breaks
                // - since the semaphore counts the not-empty threads-
                // should 'work' on 1 container for each 'post'
            }

            pthread_mutex_lock(&MapContainer_mutex[it->first]);
            for(int cont=10 ; cont>0 ;cont--)//checks the shuffle wont 'over do' and shuffle more then 10 items
                // (means the semaphore is +1)
            {
                if(it->second.size()<=0)// case the thread took less then 10 items - supposed to happen ones!
                    // TODO: check if it happens once (max for 1 thread )
                {
                    pthread_mutex_unlock(&MapContainer_mutex[it->first]);
                    break;
                }
                MAP_ITEM &p = it->second.back();
                try {
                    shuffledList.at(p.first).push_back(p.second);
                } catch (const std::out_of_range &e) {
                    shuffledList[p.first].push_back(p.second);
                }
                it->second.pop_back();
            }
            pthread_mutex_unlock(&MapContainer_mutex[it->first]);
            break;
        }
    }
}
OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase& mapReduce, IN_ITEMS_VEC& itemsVec,
                                    int multiThreadLevel, bool autoDeleteV2K2)
{
    _itemsVec = itemsVec;
    _mapReduce = &mapReduce;
    pthread_mutex_init(&pthreadToContainer_mutex, NULL);
    pthread_mutex_init(&popIndex_mutex, NULL);
    sem_init(&ShuffleSemaphore, 0 ,0);// the first 0 is correct?

    popIndex = (int)itemsVec.size();// no need to lock since there are no threads yet..


    pthread_mutex_lock(&pthreadToContainer_mutex);
    // to create the map threads which starts with 2 lines of lock and unlock pthreadToContainer_mutex
    for(int i = 0 ; i< multiThreadLevel; i++)//if itemsVec size is <10
    {
        if(pthread_create(&ExecMap[i], NULL, ExecMapJob,  NULL)!= 0 )
        {
//            print an error message : "MapReduceFramework Failure: FUNCTION_NAME failed.", where FUNCTION_NAME is the
//            name of the library call that was failed [e.g. "new"].
//            pthread_mutex_destroy(&pthreadToContainer_mutex);
//            pthread_mutex_destroy(&popIndex_mutex);
            exit(1);//destroy all threads+ map pthreadToContainer?
        }
        pthreadToContainer[ExecMap[i]] = std::vector<MAP_ITEM>();
        pthread_mutex_t mut;
        pthread_mutex_init(&mut, NULL);
        MapContainer_mutex[ExecMap[i]] = mut;
    }

    StupidVar = false;

    if(pthread_create(&shuffleThread, NULL, ExecShuffle,  NULL)!= 0 )
    {
//        pthread_mutex_destroy(&pthreadToContainer_mutex);
//        pthread_mutex_destroy(&popIndex_mutex);
        exit(1);//destroy all threads+ map pthreadToContainer?
    }
    pthread_mutex_unlock(&pthreadToContainer_mutex);
    pthread_mutex_destroy(&pthreadToContainer_mutex);          //TODO: destroy now??
    for(int i = 0; i<multiThreadLevel;i++)
    {
        pthread_mutex_t mu = MapContainer_mutex[ExecMap[i]];// ExecMap[i] gives me the thread ?
        pthread_join(ExecMap[i],NULL);
        pthread_mutex_destroy(&mu);

    }
    StupidVar = true;
    sem_post(&ShuffleSemaphore);
    pthread_join(shuffleThread,NULL);
    sem_destroy(&ShuffleSemaphore);
    //starts the reduce part
    for(int i = 0 ; i< multiThreadLevel; i++)//after the map work
    {
//        pthread_create(&ExecReduce[i], NULL, ExecReduceJob,  NULL);

    }





}

void Emit2 (k2Base* k2, v2Base* v2)
{
    pthread_mutex_lock(&MapContainer_mutex[pthread_self()]);
    pthreadToContainer[pthread_self()].push_back(std::make_pair(k2, v2));
    pthread_mutex_unlock(&MapContainer_mutex[pthread_self()]);
    sem_post(&ShuffleSemaphore);
}
void Emit3 (k3Base*, v3Base*)
{}
