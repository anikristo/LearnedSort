#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd ${DIR}
if [ ! -d "${DIR}/build" ] 
then 
mkdir build 
fi
cd build
cmake ..
make -j4
cd ..
