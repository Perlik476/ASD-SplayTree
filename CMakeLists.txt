cmake_minimum_required(VERSION 3.23)
project(ASD_SplayTree)
include(CTest)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(SplayTree
        splay.h
        tests/tests.cpp
        tests/assert.h)

add_test(SplayTreeTest
        SplayTree)