cmake_minimum_required(VERSION 3.23)
project(ASD_SplayTree)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(ASD_SplayTree
        splay.h
        tests/tests.cpp
        tests/assert.h)
