#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
EXEC="${DIR}/build/bin/LearnedSort_bench_real"

if [ ! -f "${EXEC}" ] 
then 
./compile.sh
fi

echo -e "\033[34;1mDropping caches... \033[0m[Ctrl-C to skip]"
sudo sh -c "sync; echo 1 > /proc/sys/vm/drop_caches"
${EXEC} --benchmark_display_aggregates_only
