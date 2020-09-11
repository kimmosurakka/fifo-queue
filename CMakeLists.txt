cmake_minimum_required(VERSION 3.10)

project(fifo-queue)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(fifotest
               src/fifotest.cc
               src/fifo_queue.cc)

target_include_directories(fifotest PRIVATE include)
