cmake_minimum_required(VERSION 3.22)
project(asioserver)
find_package(Boost 1.40 REQUIRED)
find_library(PQXX_LIB pqxx REQUIRED)
find_library(PQ_LIB pq REQUIRED)
find_package(libpqxx REQUIRED)

if(APPLE)
    link_directories(
            /usr/local/lib
            /usr/lib
    )
    INCLUDE_DIRECTORIES(
            /usr/local/include/
            /usr/include
    )
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq")


add_executable(asioserver main.cpp dbconnector.cpp dbconnector.h)
target_link_libraries(asioserver PUBLIC ${PQXX_LIB} ${PQ_LIB})


