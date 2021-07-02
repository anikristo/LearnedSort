cmake_minimum_required(VERSION 3.2)
project(radix_sort)

set(CMAKE_CXX_STANDARD 20)

add_library(radix_sort SHARED "radix_sort.cpp")
target_include_directories(radix_sort PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)