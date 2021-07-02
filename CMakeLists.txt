cmake_minimum_required(VERSION 3.13)
project(LearnedSort)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -fopt-info-vec-optimized")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -O3  -march=native -fno-strict-overflow -mavx")
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)

# Sorting algorithms
include_directories(${PROJECT_SOURCE_DIR}/third_party/ips4o)
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/googletest)
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/googlebenchmark)
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/radix_sort)
include_directories(${PROJECT_SOURCE_DIR}/third_party/cpp-TimSort/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/BlockQuicksort)
include_directories(${PROJECT_SOURCE_DIR}/third_party/pdqsort)
include_directories(${PROJECT_SOURCE_DIR}/third_party/ska_sort)

# Learned Sort library
include_directories(${PROJECT_SOURCE_DIR}/include)

# Shared linking dependencies
link_libraries(radix_sort)

# Synthetic benchmarks
set(BENCH_SYNTH ${CMAKE_PROJECT_NAME}_bench_synth)
add_executable(${BENCH_SYNTH} src/main_synth.cc)
target_link_libraries(${BENCH_SYNTH} PRIVATE benchmark)
install(TARGETS ${BENCH_SYNTH} DESTINATION bin)

# Real benchmarks
set(BENCH_REAL ${CMAKE_PROJECT_NAME}_bench_real)
add_executable(${BENCH_REAL} src/main_real.cc)
target_link_libraries(${BENCH_REAL} PRIVATE benchmark)
install(TARGETS ${BENCH_REAL} DESTINATION bin)

# Tests
set(TESTS ${CMAKE_PROJECT_NAME}_tests)
file(GLOB TEST_SRC "unit_tests/*.cc")
add_executable(${TESTS}  ${TEST_SRC})
add_test(NAME ${TESTS} COMMAND ${TESTS})
target_link_libraries(${TESTS} PRIVATE gtest)
install(TARGETS ${TESTS} DESTINATION tests)
