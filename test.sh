#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

if [ ! -f "${DIR}/build/bin/LearnedSort_tests" ] 
then 
./compile.sh
fi

${DIR}/third_party/gtest-parallel/gtest-parallel ${DIR}/build/bin/LearnedSort_tests --repeat=100 --workers=32