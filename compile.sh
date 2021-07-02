#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
NUM_CPUS="$(getconf _NPROCESSORS_ONLN)"
TARGETS="LearnedSort_bench_real LearnedSort_bench_synth LearnedSort_tests"

cd ${DIR}

if [ ! -d "${DIR}/build" ] 
then 
mkdir build 
fi
cd build
cmake -D CMAKE_CXX_COMPILER=g++ --target ${TARGETS} ..
make -j${NUM_CPUS} ${TARGETS}
cd ..
