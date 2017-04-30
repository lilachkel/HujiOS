cmake_minimum_required(VERSION 2.6)
set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wextra -Wall -Wvla -std=gnu++11")

#ex1
#add_library(osm ex1/osm.cpp ex1/funcplaceholder.cpp ex1/funcplaceholder.h)
#add_executable(simpletest ex1/simpletest.cpp)
#target_link_libraries(simpletest osm)

#ex2
#add_library(uthreads ex2/uthreads.cpp)
#add_library(Thread ex2/Thread.cpp)
#target_link_libraries(uthreads Thread)

#ex3
add_library(MapReduceClient ex3/MapReduceClient.cpp)
add_library(MapReduceFramework ex3/MapReduceFramework.cpp)
add_executable(Search ex3/Search.cpp)
target_link_libraries(Search MapReduceFramework)

