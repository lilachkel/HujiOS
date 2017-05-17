
#include <map>
#include <semaphore.h>
#include <unordered_map>
#include <sys/time.h>
#include "MapReduceFramework.h"
#include "Logger.h"

#define DEBUG true

IN_ITEMS_VEC _itemsVec;
OUT_ITEMS_VEC _outputVec;
MapReduceBase *_mapReduce;
std::vector<pthread_t> ExecMap;
std::vector<pthread_t> ExecReduce;

typedef std::pair<k2Base *, v2Base *> MAP_ITEM;
typedef std::pair<k2Base *, V2_VEC> SHUFFLED_ITEM;
typedef std::vector<MAP_ITEM> MAP_CONTAINER;
std::vector<SHUFFLED_ITEM> _shuffleVec;

std::unordered_map<pthread_t, MAP_CONTAINER> _pthreadToContainer;//container of <K2,V2> after the ExecMap job
std::unordered_map<pthread_t, OUT_ITEMS_VEC> _reducersContainer;
std::unordered_map<k2Base *, V2_VEC> _shuffledList;
std::unordered_map<pthread_t, pthread_mutex_t> _mapContainerMutexes;
std::unordered_map<pthread_t, pthread_mutex_t> _reduceContainerMutexes;

int popIndex;
bool StupidVar;
pthread_mutex_t pthreadToContainer_mutex;
pthread_mutex_t popIndex_mutex;
pthread_mutex_t _outputVecMutex;
pthread_t shuffleThread;
sem_t ShuffleSemaphore;

Logger _logger = Logger(".MapReduceFrameworkLog", DEBUG);


void *ExecMapJob(void *mapReduce)
{
    _logger.Log("ExecMap", false, _logger.ThreadInit);
    pthread_mutex_lock(&pthreadToContainer_mutex);
    pthread_mutex_unlock(&pthreadToContainer_mutex);
    int chunk_ind;

    while (true)
    {
        int i;
        pthread_mutex_lock(&popIndex_mutex);
        if (popIndex == -1)
        {
            pthread_mutex_unlock(&popIndex_mutex);
            _logger.Log("ExecMap", false, _logger.ThreadDeath);
            return nullptr;
        }
        chunk_ind = popIndex;
        if (popIndex - 10 > 0)
        {
            popIndex -= 10;
            pthread_mutex_unlock(&popIndex_mutex);
            i = 10;
        }
        else
        {
            i = chunk_ind - popIndex;
            popIndex = -1;
            pthread_mutex_unlock(&popIndex_mutex);// did it twice and not
            // under the ifelse since i wanted to unlock the mutex before i change 'i' to -1,
            // so that other threads will continue
        }
        for (; i >= 0; i--)
        {
            std::cout << _itemsVec[chunk_ind].first << std::endl;
//            std::cout << "gdsgfzsads\n";
            _mapReduce->Map(_itemsVec[chunk_ind].first, _itemsVec[chunk_ind].second);
            chunk_ind -= 1;
        }
    }
    _logger.Log("ExecMap", false, _logger.ThreadDeath);
}

void *ExecReduceJob(void *mapReduce)
{
    _logger.Log("ExecReduce", false, _logger.ThreadInit);
    pthread_mutex_lock(&_outputVecMutex);
    pthread_mutex_unlock(&_outputVecMutex);
    int chunkIdx;

    while (true)
    {
        int i;
        pthread_mutex_lock(&popIndex_mutex);
        if (popIndex == -1)
        {
            pthread_mutex_unlock(&popIndex_mutex);
            _logger.Log("ExecDeath", false, _logger.ThreadDeath);
            pthread_exit(nullptr);
        }

        chunkIdx = popIndex;
        if (popIndex - 10 > 0)
        {
            popIndex -= 10;
            pthread_mutex_unlock(&popIndex_mutex);
            i = 10;
        }

        else
        {
            i = chunkIdx - popIndex;
            popIndex = -1;
            pthread_mutex_unlock(&popIndex_mutex);
        }

        for (; i >= 0; i--)
        {
            _mapReduce->Reduce(_shuffleVec[chunkIdx].first, _shuffleVec[chunkIdx].second);
            chunkIdx -= 1;
        }
    }
}

void *ExecShuffle(void *mapReduce)
{
    _logger.Log("ExecShuffle", false, _logger.ThreadInit);
    int sem_val = 0;
    while (true)
    {
        sem_wait(&ShuffleSemaphore);
        sem_getvalue(&ShuffleSemaphore, &sem_val);
        if (StupidVar && sem_val == 0)
        {
            for (auto &_key : _shuffledList)
            {
                _shuffleVec.push_back(std::make_pair(_key.first, _key.second));// pushes a new pair?
                // delete such new objects i created?
                _shuffledList.erase(_key.first);// check the iterator support the deletion
            }
            _shuffledList.clear();//maybe not?

            _logger.Log("ExecShuffle", false, _logger.ThreadDeath);
            return nullptr;
        }
        for (auto &it : _pthreadToContainer)
        {
            if (it.second.size() <= 0)// not lock by mutex since the case which it gets the 'wrong' results-
                // the 'post' call called by other thread -  check if true
            {
                continue;//search the first not-empty container and breaks
                // - since the semaphore counts the not-empty threads-
                // should 'work' on 1 container for each 'post'
            }

            pthread_mutex_lock(&_mapContainerMutexes[it.first]);
            for (int cont = 10; cont > 0; cont--)//checks the shuffle wont 'over do' and shuffle more then 10 items
                // (means the semaphore is +1)
            {
                if (it.second.size() <= 0)// case the thread took less then 10 items - supposed to happen ones!
                    // TODO: check if it happens once (max for 1 thread )
                {
                    pthread_mutex_unlock(&_mapContainerMutexes[it.first]);
                    break;
                }

                MAP_ITEM &p = it.second.back();
                try
                {
                    _shuffledList[p.first].push_back(p.second);
                }
                catch (const std::out_of_range &e)
                {
                    _shuffledList[p.first] = V2_VEC();
                    _shuffledList[p.first].push_back(p.second);
                }
                it.second.pop_back();
            }
            pthread_mutex_unlock(&_mapContainerMutexes[it.first]);
            break;
        }
    }
}

void InitMapJobs(int multiThreadLevel)
{
    popIndex = (int) _itemsVec.size() - 1;// no need to lock since there are no threads yet..

    pthread_mutex_lock(&pthreadToContainer_mutex);
    // to create the map threads which starts with 2 lines of lock and unlock pthreadToContainer_mutex
    for (int i = 0; i < multiThreadLevel; i++)//if itemsVec size is <10
    {
        pthread_t thre;
        if (pthread_create(&thre, NULL, ExecMapJob, NULL) != 0)
        {
//            print an error message : "MapReduceFramework Failure: FUNCTION_NAME failed.", where FUNCTION_NAME is the
//            name of the library call that was failed [e.g. "new"].
//            pthread_mutex_destroy(&pthreadToContainer_mutex);
//            pthread_mutex_destroy(&popIndex_mutex);
            exit(EXIT_FAILURE);//destroy all threads+ map pthreadToContainer?
        }
        ExecMap.push_back(thre);
        _pthreadToContainer[ExecMap[i]] = std::vector<MAP_ITEM>();
        pthread_mutex_t mut;
        pthread_mutex_init(&mut, NULL);
        _mapContainerMutexes[ExecMap[i]] = mut;
    }
}

void InitShuffleJob(int multiThreadLevel)
{
    StupidVar = false;

    if (pthread_create(&shuffleThread, NULL, ExecShuffle, NULL) != 0)
    {
//        pthread_mutex_destroy(&pthreadToContainer_mutex);
//        pthread_mutex_destroy(&popIndex_mutex);
        exit(EXIT_FAILURE);//destroy all threads+ map pthreadToContainer?
    }
    pthread_mutex_unlock(&pthreadToContainer_mutex);
    pthread_mutex_destroy(&pthreadToContainer_mutex);          //TODO: destroy now??
    for (int i = 0; i < multiThreadLevel; i++)
    {
        pthread_mutex_t mu = _mapContainerMutexes[ExecMap[i]];// ExecMap[i] gives me the thread ?
        pthread_join(ExecMap[i], NULL);
        pthread_mutex_destroy(&mu);
    }
    _mapContainerMutexes.clear();//memory and ect
    StupidVar = true;
    sem_post(&ShuffleSemaphore);
    pthread_join(shuffleThread, NULL);
    sem_destroy(&ShuffleSemaphore);
//    for (SHUFFLED_ITEM pair:_shuffleVec)
//    {
//        auto _key = dynamic_cast<const FileNameKey *const>(pair.first);
//        std::cout << _key->GetData() << " ";
//        for(v2Base* v:pair.second)
//        {
//            auto _val = dynamic_cast<const FileNameKey *const>(v);
//            std::cout << _val->GetData() << " ";
//        }
//
//    }
}

void InitReduceJobs(int multiThreadLevel)
{
    popIndex = (int) _shuffleVec.size();
    pthread_mutex_lock(&pthreadToContainer_mutex);
    for (int i = 0; i < multiThreadLevel; i++)
    {
        if (pthread_create(&ExecReduce[i], NULL, ExecReduceJob, NULL) != 0)
        {
            _logger.Log("Failed to create new threads.", true);
            exit(EXIT_FAILURE);
        }

        _reducersContainer[ExecReduce[i]] = OUT_ITEMS_VEC();
        pthread_mutex_t m;
        pthread_mutex_init(&m, NULL);
        _reduceContainerMutexes[ExecReduce[i]] = m;
    }

    pthread_mutex_unlock(&pthreadToContainer_mutex);
    for (int i = 0; i < multiThreadLevel; i++)//after the map work
    {
        pthread_mutex_t m = _reduceContainerMutexes[ExecReduce[i]];
        pthread_join(ExecReduce[i], NULL);
        pthread_mutex_destroy(&m);
    }
}

template<typename Func>
double MeasureTime(Func op, int multiThreadLevel)
{
    struct timeval s, e;
    size_t i = 0;

    gettimeofday(&s, nullptr);
    op(multiThreadLevel);
    gettimeofday(&e, nullptr);

    return e.tv_usec - s.tv_usec;
}

void DestroyK2V2()
{
    for (auto &item : _pthreadToContainer)
    {
        for (auto &key : item.second)
        {
            delete key.first;
            delete key.second;
        }
    }
}


OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase &mapReduce, IN_ITEMS_VEC &itemsVec,
                                    int multiThreadLevel, bool autoDeleteV2K2)
{
    _logger.Log("RunMapReduceFramework started with " + std::to_string(multiThreadLevel) +
                " threads", false, _logger.ThreadDeath);

    _itemsVec = itemsVec;
    _mapReduce = &mapReduce;
    pthread_mutex_init(&pthreadToContainer_mutex, NULL);
    pthread_mutex_init(&popIndex_mutex, NULL);
    sem_init(&ShuffleSemaphore, 0, 0);// the first 0 is correct?

    //region Perform MapReduce
    // Initiate the Mapping phase
    _logger.Log(" Map and Shuffle took " +
                std::to_string(MeasureTime(InitMapJobs, multiThreadLevel) +
                               MeasureTime(InitShuffleJob, multiThreadLevel)) + " ns");

    //Initiate the Reduce phase
    _logger.Log("Reduce took " + std::to_string(MeasureTime(InitReduceJobs, multiThreadLevel)) + " ns");
    //endregion

    //region Destruction
    pthread_mutex_destroy(&pthreadToContainer_mutex);          //TODO: destroy now??

    if (autoDeleteV2K2) DestroyK2V2();
    _pthreadToContainer.clear();
    //endregion
    _logger.Log("RunMapReduceFramework finished");

    return _outputVec;
}

void Emit2(k2Base *k2, v2Base *v2)
{
    pthread_mutex_lock(&(_mapContainerMutexes[pthread_self()]));
    _pthreadToContainer[pthread_self()].push_back({k2, v2});
    pthread_mutex_unlock(&(_mapContainerMutexes[pthread_self()]));
    sem_post(&ShuffleSemaphore);
}

void Emit3(k3Base *k3, v3Base *v3)
{
    pthread_mutex_lock(&(_reduceContainerMutexes[pthread_self()]));
    _reducersContainer[pthread_self()].push_back({k3, v3});
    pthread_mutex_unlock(&(_reduceContainerMutexes[pthread_self()]));
}
