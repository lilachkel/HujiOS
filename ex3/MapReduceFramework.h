#ifndef MAPREDUCEFRAMEWORK_H
#define MAPREDUCEFRAMEWORK_H

#include "MapReduceClient.h"
#include <pthread.h>
#include <utility>
#include <vector>
#include <map>
#include <semaphore.h>

typedef std::pair<k1Base*, v1Base*> IN_ITEM;
typedef std::pair<k3Base*, v3Base*> OUT_ITEM;

typedef std::pair<k2Base*, v2Base*> MAP_ITEM;

typedef std::vector<IN_ITEM> IN_ITEMS_VEC; 
typedef std::vector<OUT_ITEM> OUT_ITEMS_VEC;

typedef std::vector<MAP_ITEM> MAP_CONTAINER;




OUT_ITEMS_VEC RunMapReduceFramework(MapReduceBase& mapReduce, IN_ITEMS_VEC& itemsVec, 
									int multiThreadLevel, bool autoDeleteV2K2);

void Emit2 (k2Base*, v2Base*);
void Emit3 (k3Base*, v3Base*);

void *ExecMapJob(MapReduceBase& mapReduce);

#endif //MAPREDUCEFRAMEWORK_H
