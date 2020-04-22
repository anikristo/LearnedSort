#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

if [ ! -f "${DIR}/build/bin/LearnedSort_tests" ] 
then 
./compile.sh
fi

${DIR}/build/bin/LearnedSort_tests --gtest_shuffle --gtest_filter=*.StdNormal --test_size=1000000