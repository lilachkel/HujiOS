set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wextra -Wall -Wvla -std=gnu++11")

add_library(osm ex1/osm.cpp)
add_executable(simpletest ex1/simpletest.cpp)
target_link_libraries(simpletest osm)
