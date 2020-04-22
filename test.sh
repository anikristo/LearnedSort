#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

if [ ! -f "${DIR}/build/bin/LearnedSort_tests" ] 
then 
./compile.sh
fi

${DIR}/external/gtest-parallel/gtest-parallel ${DIR}/build/bin/LearnedSort_tests --repeat=16 --workers=16