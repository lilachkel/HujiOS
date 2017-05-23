//#define NDEBUG

#include <map>
#include <semaphore.h>
#include <unordered_map>
#include <sys/time.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "MapReduceFramework.h"

#include "ex_3_test_suite/Test_28/WordFrequenciesClient.hpp"

void SafePrint(std::string msg);

#define LOG_THREAD_CREATION(t) SafePrint("Thread " + std::string(t) + " created [" + GetTimeString() + "]");
#define LOG_THREAD_TERMINATION(t) SafePrint("Thread " + std::string(t) + " terminated [" + GetTimeString() + "]");

struct Comparator
{
    bool operator()(k2Base *a, k2Base *b) const
    { return *a < *b; }
};

std::map<k2Base *, V2_VEC>::iterator _shuffleIter;

IN_ITEMS_VEC _itemsVec;
OUT_ITEMS_VEC _outputVec;
MapReduceBase *_mapReduce;
std::vector<pthread_t> ExecMap;
std::vector<pthread_t> ExecReduce;

typedef std::pair<k2Base *, v2Base *> MAP_ITEM;
typedef std::vector<MAP_ITEM> MAP_CONTAINER;

std::unordered_map<pthread_t, MAP_CONTAINER> _pthreadToContainer;
std::unordered_map<pthread_t, pthread_mutex_t> _mapContainerMutexes;
std::map<k2Base *, V2_VEC, Comparator> _shuffledMap;
std::unordered_map<pthread_t, OUT_ITEMS_VEC> _reducersContainer;

int popIndex;
bool _isMappingFinished;
sem_t ShuffleSemaphore;
pthread_t _shuffleThread;
pthread_mutex_t _execMapMutex;
pthread_mutex_t _execReduceMutex;
pthread_mutex_t pthreadToContainer_mutex;
pthread_mutex_t popIndex_mutex;
pthread_mutex_t _logMutex;

std::ofstream _log;

/**
 * Outputs to the cout while eliminating race conditions using a mutex.
 * @param msg the string we want to output
 */
void SafePrint(std::string msg)
{
    pthread_mutex_lock(&_logMutex);
    std::cout << msg << std::endl;
    std::flush(std::cout);
    pthread_mutex_unlock(&_logMutex);
}

/**
 * Closes the file stream and quits with a 1
 * @param msg eror message
 */
void QuitWithError(std::string msg)
{
    std::cerr << msg << std::endl;
    _log.close();
    exit(EXIT_FAILURE);
}

/**
 * Time measurement function
 * @param op function we want to measure its runtime
 * @param multiThreadLevel number of threads should be created
 * @return time measurment in ns
 */
template<typename Func>
size_t MeasureTime(Func op, int multiThreadLevel)
{
    struct timeval s, e;

    gettimeofday(&s, nullptr);
    op(multiThreadLevel);
    gettimeofday(&e, nullptr);

    size_t sec = (e.tv_sec - s.tv_sec) * 1000000;
    size_t usec = (e.tv_usec - s.tv_usec) * 1000;

    return sec + usec;
}

/**
 * Returns a formatted string repressenting the current date and time
 * @return date-time string with a special format
 */
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

void *ExecMapJob(void *arg)
{
    pthread_mutex_lock(&_execMapMutex);
    pthread_mutex_unlock(&_execMapMutex);
    LOG_THREAD_CREATION("ExecMap")
    int chunkSize, chunkStart;

    while ((int) _itemsVec.size() > popIndex)
    {
        pthread_mutex_lock(&popIndex_mutex);
        chunkSize = std::min(10, (int) _itemsVec.size() - popIndex);
        chunkStart = popIndex;
        popIndex += chunkSize;
        pthread_mutex_unlock(&popIndex_mutex);

        for (int i = chunkStart; i < chunkStart + chunkSize; i++)
        {
            auto &item = _itemsVec[i];
            _mapReduce->Map(item.first, item.second);
        }
    }

    LOG_THREAD_TERMINATION("ExecMap")
    pthread_exit(nullptr);
}

void *ExecReduceJob(void *arg)
{
    pthread_mutex_lock(&_execReduceMutex);
    pthread_mutex_unlock(&_execReduceMutex);
    LOG_THREAD_CREATION("ExecReduce")
    int chunkSize;

    while ((int) _shuffledMap.size() > popIndex)
    {
        pthread_mutex_lock(&popIndex_mutex);
        auto start = _shuffleIter;
        chunkSize = std::min(10, (int) _shuffledMap.size() - popIndex);
        std::advance(_shuffleIter, chunkSize);
        auto end = _shuffleIter;
        popIndex += chunkSize;
        pthread_mutex_unlock(&popIndex_mutex);

        std::for_each(start, end,
                      [](auto &item) {
                          _mapReduce->Reduce(item.first, item.second);
                      });
    }

    LOG_THREAD_TERMINATION("ExecReduce")
    pthread_exit(nullptr);
}

/**
 * Shuffle thread function
 */
void *ExecShuffle(void *mapReduce)
{
    sem_wait(&ShuffleSemaphore);
    sem_post(&ShuffleSemaphore);
    LOG_THREAD_CREATION("ExecShuffle")

    while (!_isMappingFinished)
    {
        sem_wait(&ShuffleSemaphore);
        for (auto &it : _pthreadToContainer)
        {
            if (it.second.size() == 0)
                continue;

            pthread_mutex_lock(&_mapContainerMutexes[it.first]);
            auto &p = it.second.back();
            _shuffledMap[p.first].push_back(p.second);
            it.second.pop_back();
            pthread_mutex_unlock(&_mapContainerMutexes[it.first]);
        }
    }
    // if all map threads finished we can just empty the containers without lock-unlocking the mutex
    for (auto &it : _pthreadToContainer)
    {
        for (auto &v: it.second)
        {
            _shuffledMap[v.first].push_back(v.second);
        }
    }
    LOG_THREAD_TERMINATION("ExecShuffle")
    pthread_exit(nullptr);
}

/**
 * Creates and initializes given number of Map threads and one shuffle thread
 * @param multiThreadLevel number of map threads to create
 */
void InitMapShuffleJobs(int multiThreadLevel)
{
    //region Init
    pthread_mutex_init(&_execMapMutex, NULL);
    sem_init(&ShuffleSemaphore, 0, 0);// the first 0 is correct?
    _isMappingFinished = false;
    popIndex = 0;// no need to lock since there are no threads yet..
    ExecMap.reserve(multiThreadLevel);
    //endregion

    //region Create Map threads
    pthread_mutex_lock(&_execMapMutex);
    for (int i = 0; i < multiThreadLevel; i++)//if itemsVec size is <10
    {
        if (pthread_create(&ExecMap[i], NULL, ExecMapJob, NULL) != 0)
        {
            QuitWithError("Failed to create Map threads.");
        }
        _pthreadToContainer[ExecMap[i]] = std::vector<MAP_ITEM>();
        pthread_mutex_init(&(_mapContainerMutexes[ExecMap[i]]), NULL);
    }
    pthread_mutex_unlock(&_execMapMutex);
    //endregion

    //region Create Shuffle thread
    if (pthread_create(&_shuffleThread, NULL, ExecShuffle, NULL) != 0)
    {
        QuitWithError("Failed to create shuffle thread.");
    }
    for (int i = 0; i < multiThreadLevel; i++)
    {
        pthread_join(ExecMap[i], NULL);
    }
    //endregion

    //region Wait for Map and Shuffle to finish and cleanup
    _isMappingFinished = true;
    sem_post(&ShuffleSemaphore);
    pthread_join(_shuffleThread, NULL);
    for (int i = 0; i < multiThreadLevel; i++)
    {
        pthread_mutex_destroy(&(_mapContainerMutexes[ExecMap[i]]));
    }
    sem_destroy(&ShuffleSemaphore);
    pthread_mutex_destroy(&_execMapMutex);
    //endregion
}

/**
 * Creates a given number of reduce threads.
 */
void InitReduceJobs(int multiThreadLevel)
{
    //reguib Init
    popIndex = 0;
    _shuffleIter = _shuffledMap.begin();
    ExecReduce.reserve(multiThreadLevel);
    //endregion

    //region Create Reduce threads
    pthread_mutex_lock(&_execReduceMutex);
    for (int i = 0; i < multiThreadLevel; i++)
    {
        if (pthread_create(&(ExecReduce[i]), NULL, ExecReduceJob, NULL) != 0)
        {
            QuitWithError("Failed to create Reduce threads.");
        }
        _reducersContainer[ExecReduce[i]];
    }
    pthread_mutex_unlock(&_execReduceMutex);
    //endregion

    //region Wait for reducers to finish and cleanup
    for (int i = 0; i < multiThreadLevel; i++)
    {
        pthread_join(ExecReduce[i], NULL);
    }
    pthread_mutex_destroy(&_execReduceMutex);
    //endregion

    //region Combine the reducers output and sort it
    for (auto &v : _reducersContainer)
        _outputVec.insert(_outputVec.end(), v.second.begin(), v.second.end());
    std::sort(_outputVec.begin(), _outputVec.end(), [](OUT_ITEM a, OUT_ITEM b) { return *a.first < *b.first; });
    //endregion
}

/**
 * Releases resources allocated to the <k2,v2> pairs in case the autoDeleteK2V2 set to true
 */
void DestroyK2V2()
{
    for (auto &item : _pthreadToContainer)
    {
        for (auto &key : item.second)
        {
            delete key.first;
            delete key.second;
        }
        item.second.clear();
    }
}

/**
 * Initializes and runs a map reduce on a give input vector with a given number of threads.
 * @param mapReduce MapReduce object that inherits from MapReduceBase and implements the Map and Reduce functions
 * @param itemsVec input <k1,v1> vector
 * @param multiThreadLevel number of threads that should be created to process the data
 * @param autoDeleteV2K2 if true <k2,v2> pairs will be freed automatically
 * @return sorted vector of <k3,v3> pairs
 */
OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase &mapReduce, IN_ITEMS_VEC &itemsVec,
                                    int multiThreadLevel, bool autoDeleteV2K2)
{
    //region Init Framework
    std::ofstream _log;
    _log.open(".MapReduceFrameworkLog");
    if (!_log.is_open())
    {
        std::cerr << "Cannot open log file for writing!" << std::endl;
        exit(EXIT_FAILURE);
    }

#ifdef NDEBUG
    auto backup = std::cout.rdbuf();
    std::cout.rdbuf(_log.rdbuf());
#endif

    std::cout << "RunMapReduceFramework started with " + std::to_string(multiThreadLevel) + " threads." << std::endl;
    _itemsVec = itemsVec;
    _mapReduce = &mapReduce;
    pthread_mutex_init(&_execReduceMutex, NULL);
    pthread_mutex_init(&popIndex_mutex, NULL);
    pthread_mutex_init(&_logMutex, NULL);
    //endregion

    //region Perform MapReduce
    // Initiate the Mapping and Shuffling phases
    try
    {
        size_t mapTime = MeasureTime(InitMapShuffleJobs, multiThreadLevel);
        std::cout << "Map and Shuffle took " +
                     std::to_string(mapTime) + "ns" << std::endl;

        //Initiate the Reduce phase
        size_t reduceTime = MeasureTime(InitReduceJobs, multiThreadLevel);
        std::cout << "Reduce took " +
                     std::to_string(reduceTime) + "ns" << std::endl;
    }
    catch (std::exception e)
    {
        QuitWithError(e.what());
    }
    //endregion

    //region Cleanup
    pthread_mutex_destroy(&pthreadToContainer_mutex);          //TODO: destroy now??
    pthread_mutex_destroy(&popIndex_mutex);          //TODO: destroy now??

    if (autoDeleteV2K2) DestroyK2V2();

    _pthreadToContainer.clear();
    std::cout << "RunMapReduceFramework finished" << std::endl;
    //endregion

#ifdef NDEBUG
    std::cout.rdbuf(backup);
#endif
    _log.close();

    return _outputVec;
}

/**
 * Adds the given k2,v2 to the vector for later processing
 */
void Emit2(k2Base *k2, v2Base *v2)
{
    try
    {
        pthread_mutex_lock(&_mapContainerMutexes[pthread_self()]);
        _pthreadToContainer[pthread_self()].push_back({k2, v2});
        pthread_mutex_unlock(&(_mapContainerMutexes[pthread_self()]));
        sem_post(&ShuffleSemaphore);
    }
    catch (std::exception e)
    {
        std::cerr << "Emit2 failed with:" << std::endl << e.what() << std::endl;
        throw e;
    }
}

/**
 * Adds the given k3,v3 to the vector for later processing
 */
void Emit3(k3Base *k3, v3Base *v3)
{
    try
    {
        _reducersContainer[pthread_self()].push_back({k3, v3});
    }
    catch (std::exception e)
    {
        std::cerr << "Emit3 failed with:" << std::endl << e.what() << std::endl;
        throw e;
    }
}
