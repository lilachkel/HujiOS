cmake_minimum_required(VERSION 2.6)
set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wextra -Wall -Wvla -std=gnu++11")

#ex1
#add_library(osm ex1/osm.cpp ex1/funcplaceholder.cpp ex1/funcplaceholder.h)
#add_executable(simpletest ex1/simpletest.cpp)
#target_link_libraries(simpletest osm)

add_library(uthreads ex2/uthreads.cpp)
add_library(Thread ex2/Thread.cpp)
add_library(LinkedList ex2/LinkedList.cpp)
target_link_libraries(uthreads Thread LinkedList)
add_executable(demo_jmp ex2/demo_jmp.c)
add_executable(demo_itimer ex2/demo_itimer.c)
add_executable(demo_singInt_handler ex2/demo_singInt_handler.c)
target_link_libraries(demo_jmp uthreads)
target_link_libraries(demo_itimer uthreads)
target_link_libraries(demo_singInt_handler uthreads)
