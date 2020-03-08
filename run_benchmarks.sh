#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
${DIR}/external/googlebenchmark/tools/compare.py filters ${DIR}/build/bin/LearnedSort_benchmarks Benchmarks/StdSort Benchmarks/IS4o