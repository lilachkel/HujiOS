cmake_minimum_required(VERSION 3.5)
set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wall -std=c++11 ")

#ex1
#add_library(osm ex1/osm.cpp ex1/funcplaceholder.cpp ex1/funcplaceholder.h)
#add_executable(simpletest ex1/simpletest.cpp)
#target_link_libraries(simpletest osm)

#ex2
add_library(uthreads ex2/uthreads.cpp)
add_library(Thread ex2/Thread.cpp)
target_link_libraries(uthreads Thread)

#ex3
add_library(MapReduceFramework ex3/MapReduceFramework.cpp ex3/MapReduceClient.h ex3/Logger.cpp ex3/Logger.h
        ex3/SearchMapReduce.cpp ex3/FileNameKey.hpp)
add_executable(Search ex3/Search.cpp)
target_link_libraries(Search MapReduceFramework -lstdc++fs -pthread)

