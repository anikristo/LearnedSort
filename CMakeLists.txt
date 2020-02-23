cmake_minimum_required(VERSION 3.2)
project(LearnedSort)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tst)
add_subdirectory(lib/googletest)