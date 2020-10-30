cmake_minimum_required(VERSION 3.17)
project(post_c)

set(CMAKE_CXX_STANDARD 17)

add_executable(posts_cpp main.cpp FixedBitSet.cpp FixedBitSet.h)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(" ")
    message("CMAKE IN DEBUG MODE")
    message(" ")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(" ")
    message("CMAKE IN RELEASE MODE")
    message(" ")
endif ()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -DBOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS")
set(CMAKE_CXX_FLAGS_DEBUG "-g -mavx2 -DSAFETY_CHECKS")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mavx2")
