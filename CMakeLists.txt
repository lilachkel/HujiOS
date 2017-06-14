cmake_minimum_required(VERSION 3.5)
set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wall -std=c++11 ")

#ex1
#add_library(osm ex1/osm.cpp ex1/funcplaceholder.cpp ex1/funcplaceholder.h)
#add_executable(simpletest ex1/simpletest.cpp)
#target_link_libraries(simpletest osm)

#ex2
#add_library(uthreads ex2/uthreads.cpp)
#add_library(Thread ex2/Thread.cpp ex2/tet.cpp ex2/tet.h)
#target_link_libraries(uthreads Thread)

#ex3
#add_library(MapReduceFramework ex3/MapReduceFramework.cpp)
#add_library(SearchMapReduce ex3/SearchMapReduce.cpp ex3/FileNameKey.hpp ex3/MapReduceClient.h)
#add_executable(Search ex3/Search.cpp)
#target_link_libraries(Search MapReduceFramework SearchMapReduce -lstdc++fs -pthread)
#
#add_library(GridMapReduce ex3/ex_3_test_suite/Test_6/GridMapReduce.hpp
#        ex3/ex_3_test_suite/Test_6/GridKeysValues.hpp)
#set_target_properties(GridMapReduce PROPERTIES LINKER_LANGUAGE CXX)
#add_executable(Test6 ex3/ex_3_test_suite/Test_6/GridShamansFinder.cpp)
#target_link_libraries(Test6 MapReduceFramework GridMapReduce -lstdc++fs -pthread)
#
#add_library(WordFrequenciesClient ex3/ex_3_test_suite/Test_28/WordFrequenciesClient.hpp)
#set_target_properties(WordFrequenciesClient PROPERTIES LINKER_LANGUAGE CXX)
#add_executable(Test28 ex3/ex_3_test_suite/Test_28/WordFrequencies.cpp)
#target_link_libraries(Test28 WordFrequenciesClient MapReduceFramework -lstdc++fs -pthread)
#
#add_executable(PrimeTest ex3/ex_3_test_suite/TestGuy/PrimeTest.cpp)
#target_link_libraries(PrimeTest MapReduceFramework SearchMapReduce -lstdc++fs -pthread)
#
#add_executable(ShaharTest ex3/ex_3_test_suite/TestShahar/test.cpp)
#target_link_libraries(ShaharTest MapReduceFramework SearchMapReduce -lstdc++fs -pthread)
#
#add_executable(WordFrequencies ex3/ex_3_test_suite/Test_28/WordFrequencies.cpp)
#target_link_libraries(WordFrequencies MapReduceFramework SearchMapReduce -lstdc++fs -pthread)

# ex4
add_library(Algorithms ex4/ICacheAlgorithm.hpp ex4/LruAlgorithm.cpp ex4/LfuAlgorithm.cpp ex4/FbrAlgorithm.cpp)
set_target_properties(Algorithms PROPERTIES LINKER_LANGUAGE CXX)
add_library(CacheFS ex4/CacheFS.cpp)
target_link_libraries(CacheFS Algorithms)
add_executable(Test1 ex4/Tests/TestFiles/Test1.cpp)
add_executable(Test2 ex4/Tests/TestFiles/Test2.cpp)
add_executable(Test3 ex4/Tests/TestFiles/Test3.cpp)
add_executable(Test4 ex4/Tests/TestFiles/Test4.cpp)
add_executable(Test5 ex4/Tests/TestFiles/Test5.cpp)
add_executable(TestLFU ex4/TestLFU.cpp)
target_link_libraries(TestLFU CacheFS)
target_link_libraries(Test1 CacheFS)
target_link_libraries(Test2 CacheFS)
target_link_libraries(Test3 CacheFS)
target_link_libraries(Test4 CacheFS)
target_link_libraries(Test5 CacheFS)