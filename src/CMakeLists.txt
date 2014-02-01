cmake_minimum_required (VERSION 2.6)
project(baryon)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb -Wall -Wextra -pedantic -std=c++11")
endif()

set(PLANNER_SOURCE
	main.cpp
	definitions/state.cpp)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(baryon ${PLANNER_SOURCE})
