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
add_library(MapReduceFramework ex3/MapReduceFramework.cpp ex3/Logger.cpp)
add_library(SearchMapReduce ex3/SearchMapReduce.cpp ex3/FileNameKey.cpp)
add_executable(Search ex3/Search.cpp)
target_link_libraries(Search MapReduceFramework SearchMapReduce -lstdc++fs -pthread)

add_library(GridMapReduce ex3/ex_3_test_suite/Test_6/GridMapReduce.cpp ex3/ex_3_test_suite/Test_6/GridKeysValues.cpp)
add_executable(Test6 ex3/ex_3_test_suite/Test_6/GridShamansFinder.cpp)
target_link_libraries(Test6 MapReduceFramework GridMapReduce -lstdc++fs -pthread)

