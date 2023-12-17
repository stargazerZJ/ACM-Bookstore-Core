cmake_minimum_required(VERSION 3.22)
project(Bookstore)

if(DEFINED ENV{LOCAL})
    add_definitions(-DLOCAL)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

# Find SWIG
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# Find Python
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})

# Include your source directory
include_directories(${CMAKE_SOURCE_DIR}/src)

set_source_files_properties(src/bookstore.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(src/bookstore.i PROPERTIES SWIG_FLAGS "-includeall")
swig_add_library(Bookstore LANGUAGE python SOURCES src/bookstore.i)
swig_link_libraries(Bookstore ${PYTHON_LIBRARIES})


file(GLOB_RECURSE main_src src/*.cpp)
#add_executable(code ${main_src})
