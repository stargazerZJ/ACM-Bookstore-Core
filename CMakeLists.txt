cmake_minimum_required(VERSION 3.22)
project(Bookstore)

if(DEFINED ENV{LOCAL})
    add_definitions(-DLOCAL)
endif()

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")

file(GLOB_RECURSE main_src src/*.cpp )

add_executable(code ${main_src})