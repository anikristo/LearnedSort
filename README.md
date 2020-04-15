# Learned Sort
The official repository for Learned Sort, a model-enhanced sorting algorithm that was published in [The Case of a Learned Sorting Algorithm](#TODO). 

This repository contains:
- The Learned Sort algorithm under `lib/learned_sort.hh`
- Benchmarking code in `benchmarks_driver.cc`
- Unit testing code under `test/`

## Instructions to build project

1. Clone this repository locally using the `--recursive` flag so that it pulls the submodules too
1. Change into the new directory `cd learned-sort`
1. Run the compiling script `./compile.sh`

## Benchmarks
In order to run the benchmarks execute: `./run.sh`. 

This uses [Google Benchmark](https://www.github.com/google/benchmark) to measure the running times for Learned Sort and other sorting algorithms for comparison.
The benchmarks will be run for various input sizes, and with enough iterations to provide a stable statistic. 
At the end of the benchmark for each algorithm, Google Benchmark will provide an estimation of the coefficient for the high-order term in the time complexity and report the [RMS error](https://en.wikipedia.org/wiki/Root-mean-square_deviation). 

At the end of the benchmark execution, the console will display a pair-wise comparison of the running times of Learned Sort and IS4o.

## Tests
In order to run the tests execute: `./test.sh`. 

This uses [Google Test](https://www.github.com/google/googletest) to perform unit testing on Learned Sort and other basline sorting algorithms on various data distributions and data types. 
After downloading this repository, run the tests to make sure everything is working fine for your system setup. 
