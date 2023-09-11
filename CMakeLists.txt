cmake_minimum_required(VERSION 3.0)

project(liang_all CXX)

#include(CTest)
enable_testing()

message(STATUS "cmake project name : ${CMAKE_PROJECT_NAME}")

if(CMAKE_PROJECT_NAME STREQUAL "liang_all")
  option(LIANG_BUILD_TESTS "Build Liang tests" ON)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CXX_FLAGS
    -std=c++11
    -rdynamic
    -g

    # -DVALGRIND
    -DCHECK_PTHREAD_RETURN_VALUE
    -D_FILE_OFFSET_BITS=64
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wno-unused-parameter
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wshadow
    -Wwrite-strings
    -march=native
)

# include directories
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/spdlog-1.12.0/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/readerwriterqueue-1.0.6)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/concurrentqueue-1.0.4)

# sub library
add_subdirectory(core)

# add_executable(pollserver main.cpp)

# set(CMAKE_COMPILER_IS_GNUCXX,TRUE)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
