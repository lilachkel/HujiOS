#define NDEBUG

#include <map>
#include <semaphore.h>
#include <pthread.h>
#include <unordered_map>
#include <sys/time.h>
#include <fstream>
#include <iostream>
#include "MapReduceFramework.h"

#define NANO_TO_SEC(x) x / 1000
#define LOG_THREAD_CREATION(t) std::cout << "Thread " << t << " created [" + GetTimeString() + "]" << std::endl;
#define LOG_THREAD_TERMINATION(t) std::cout << "Thread " << t << " terminated [" + GetTimeString() + "]" << std::endl;

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

std::ofstream _log;

void QuitWithError(std::string msg)
{
    std::cerr << msg << std::endl;
    _log.close();
    exit(EXIT_FAILURE);
}

template<typename Func>
std::pair<long, double> MeasureTime(Func op, int multiThreadLevel)
{
    struct timeval s, e;

    gettimeofday(&s, nullptr);
    op(multiThreadLevel);
    gettimeofday(&e, nullptr);

    return {e.tv_sec - s.tv_sec, NANO_TO_SEC(e.tv_usec - s.tv_usec)};
}

std::string GetTimeString()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", timeinfo);
    std::string str(buffer);

    return str;
}


void *ExecMapJob(void *mapReduce)
{
    LOG_THREAD_CREATION("ExecMap")
    if(pthread_mutex_trylock(&pthreadToContainer_mutex) != 0)
        throw "Failed to lock pthread container mutex";

    pthread_mutex_unlock(&pthreadToContainer_mutex);
    int chunk_ind;

    while (true)
    {
        int i;
        if (pthread_mutex_trylock(&popIndex_mutex) == 0)
        {
            if (popIndex == -1)
            {
                pthread_mutex_unlock(&popIndex_mutex);
                LOG_THREAD_TERMINATION("ExecMap")
                pthread_exit(nullptr);
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
                //TODO: remove this line:
                std::cout << _itemsVec[chunk_ind].first << std::endl;
//            std::cout << "gdsgfzsads\n";
                _mapReduce->Map(_itemsVec[chunk_ind].first, _itemsVec[chunk_ind].second);
                chunk_ind -= 1;
            }
        }
        else
            throw "Failed to lock popIndex mutex";
    }

    // TODO: where should i print?
    // _log << "Thread ExecMap terminated [" + GetTimeString() + "]" << std::endl;
}

void *ExecReduceJob(void *mapReduce)
{
    LOG_THREAD_CREATION("ExecReduce")
    if (pthread_mutex_trylock(&_outputVecMutex) != 0)
    {
        throw "Failed to lock output vector mutex";
    }
    pthread_mutex_unlock(&_outputVecMutex);
    int chunkIdx;

    while (true)
    {
        int i;
        if (pthread_mutex_trylock(&popIndex_mutex) == 0)
        {
            if (popIndex == -1)
            {
                pthread_mutex_unlock(&popIndex_mutex);
                LOG_THREAD_TERMINATION("ExecReduce")
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
        else
            throw "Failed to lock popindex mutex";
    }
}

void *ExecShuffle(void *mapReduce)
{
    LOG_THREAD_CREATION("ExecShuffle")

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

            LOG_THREAD_TERMINATION("ExecShuffle")
            pthread_exit(nullptr);
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

            if (pthread_mutex_trylock(&_mapContainerMutexes[it.first]) == 0)
            {
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
            else
                throw "Failed to lock one of the map container mutexes.";
        }
    }
}

void InitMapJobs(int multiThreadLevel)
{
    popIndex = (int) _itemsVec.size() - 1;// no need to lock since there are no threads yet..

    if (pthread_mutex_trylock(&pthreadToContainer_mutex) == 0)
    {
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
                //destroy all threads+ map pthreadToContainer?
                QuitWithError("Failed to create Map threads.");
            }
            ExecMap.push_back(thre);
            _pthreadToContainer[ExecMap[i]] = std::vector<MAP_ITEM>();
            pthread_mutex_t mut;
            pthread_mutex_init(&mut, NULL);
            _mapContainerMutexes[ExecMap[i]] = mut;
        }
    }
    else throw "Cannot lock pthreadContainer mutex for Map jobs";
}

void InitShuffleJob(int multiThreadLevel)
{
    StupidVar = false;

    if (pthread_create(&shuffleThread, NULL, ExecShuffle, NULL) != 0)
    {
//        pthread_mutex_destroy(&pthreadToContainer_mutex);
//        pthread_mutex_destroy(&popIndex_mutex);
        QuitWithError("Failed to create shuffle thread.");
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
    popIndex = (int) _shuffleVec.size() - 1;

    if (pthread_mutex_trylock(&pthreadToContainer_mutex) == 0)
    {
        for (int i = 0; i < multiThreadLevel; i++)
        {
            pthread_t thre;
            if (pthread_create(&thre, NULL, ExecReduceJob, NULL) != 0)
            {
                QuitWithError("Failed to create Reduce threads.");
            }
            ExecReduce.push_back(thre);

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
    else
        throw "unable to lock pthreadToContainer_mutex";
}

void DestroyK2V2()
{
    for(auto &item : _pthreadToContainer)
    {
        for(auto &key : item.second)
        {
            delete key.first;
            delete key.second;
        }
        item.second.clear();
    }
}

OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase &mapReduce, IN_ITEMS_VEC &itemsVec,
                                    int multiThreadLevel, bool autoDeleteV2K2)
{
    //region Init Framework
    std::ofstream _log;
    _log.open(".MapReduceFrameworkLog", std::ios::app);
    if (!_log.is_open())
    {
        std::cerr << "Cannot open log file for writing!" << std::endl;
        exit(EXIT_FAILURE);
    }

#ifndef NDEBUG
    auto backup = std::cout.rdbuf();
    std::cout.rdbuf(_log.rdbuf());
#endif

    std::cout << "RunMapReduceFramework started with " + std::to_string(multiThreadLevel) + " threads." << std::endl;

    _itemsVec = itemsVec;
    _mapReduce = &mapReduce;
    pthread_mutex_init(&pthreadToContainer_mutex, NULL);
    pthread_mutex_init(&popIndex_mutex, NULL);
    sem_init(&ShuffleSemaphore, 0, 0);// the first 0 is correct?
    //endregion

    //region Perform MapReduce
    // Initiate the Mapping and Shuffling phases
    try
    {
        std::pair<long, double> mapTime = MeasureTime(InitMapJobs, multiThreadLevel);
        std::pair<long, double> shuffleTime = MeasureTime(InitShuffleJob, multiThreadLevel);
        std::pair<long, double> reduceTime = MeasureTime(InitReduceJobs, multiThreadLevel);

        std::cout << " Map and Shuffle took " +
                std::to_string(mapTime.first + shuffleTime.first) + "." +
                std::to_string(mapTime.second + shuffleTime.first) + "s" << std::endl;

        //Initiate the Reduce phase
        std::cout << "Reduce took " +
                std::to_string(reduceTime.first) + "." + std::to_string(reduceTime.second) + "s" << std::endl;
    }
    catch (std::exception e)
    {

        QuitWithError(e.what());
    }


    //endregion

    //region Destruction
    pthread_mutex_destroy(&pthreadToContainer_mutex);          //TODO: destroy now??

    if (autoDeleteV2K2) DestroyK2V2();

    _pthreadToContainer.clear();
    _shuffleVec.clear();
    std::cout << "RunMapReduceFramework finished" << std::endl;
    //endregion

#ifndef NDEBUG
    std::cout.rdbuf(backup);
#endif
    _log.close();

    return _outputVec;
}

void Emit2(k2Base *k2, v2Base *v2)
{
    try
    {
        if (pthread_mutex_trylock(&(_mapContainerMutexes[pthread_self()])) == 0)
        {
            _pthreadToContainer[pthread_self()].push_back({k2, v2});
            pthread_mutex_unlock(&(_mapContainerMutexes[pthread_self()]));
            sem_post(&ShuffleSemaphore);
        }
        else throw "Cannot lock the _mapContainerMutex.";
    }
    catch (std::exception e)
    {
        std::cerr << "Emit2 failed with:" << std::endl << e.what() << std::endl;
        throw e;
    }
}

void Emit3(k3Base *k3, v3Base *v3)
{
    try
    {
        if (pthread_mutex_trylock(&(_reduceContainerMutexes[pthread_self()])) == 0)
        {
            _reducersContainer[pthread_self()].push_back({k3, v3});
            pthread_mutex_unlock(&(_reduceContainerMutexes[pthread_self()]));
        }
                else throw "Cannot lock the _reduceContainerMutex.";
    }
    catch (std::exception e)
    {
        std::cerr << "Emit3 failed with:" << std::endl << e.what() << std::endl;
        throw e;
    }
}
