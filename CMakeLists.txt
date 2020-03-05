cmake_minimum_required(VERSION 3.2)
project(LearnedSort)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -march=native -fsave-optimization-record")
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dependencies
include_directories(${PROJECT_SOURCE_DIR}/external/ips4o)
add_subdirectory(external/googletest)
add_subdirectory(external/googlebenchmark)

# Learned Sort library
include_directories(${PROJECT_SOURCE_DIR}/lib)

# Executable
set(BINARY ${CMAKE_PROJECT_NAME}_benchmarks)
add_executable(${BINARY} benchmarks_driver.cc)
target_link_libraries(${BINARY} benchmark)

# Tests
set(TESTS ${CMAKE_PROJECT_NAME}_tests)
file(GLOB TEST_SRC "test/*.cc")
add_executable(${TESTS}  ${TEST_SRC})
add_test(NAME ${TESTS} COMMAND ${TESTS})
target_link_libraries(${TESTS} PUBLIC gtest)
