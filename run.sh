#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

if [ ! -f "${DIR}/build/bin/LearnedSort_benchmarks" ] 
then 
./compile.sh
fi

${DIR}/build/bin/LearnedSort_benchmarks --benchmark_repetitions=5 --benchmark_display_aggregates_only