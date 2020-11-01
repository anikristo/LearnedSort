#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
NUM_CPUS=`nproc`

cd ${DIR}

git submodule update --init --recursive --remote

if [ ! -d "${DIR}/build" ] 
then 
mkdir build 
fi
cd build
cmake ..
make -j${NUM_CPUS}
cd ..
