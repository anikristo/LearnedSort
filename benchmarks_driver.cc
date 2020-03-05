#include <algorithm>
#include <random>
#include <benchmark/benchmark.h>

#include "ips4o.hpp"

using namespace std;

class Benchmarks : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state) {

        // 1. Initialize random engine with normal distribution
        random_device rd;
        mt19937 generator(rd());
        normal_distribution<> distribution(0, 1);

        // 2. Populate the input
        for (int i = 0; i < state.range(0); i++) {
            arr.push_back(distribution(generator));
        }
    }

    // Input array
    vector<double> arr;

};

BENCHMARK_DEFINE_F(Benchmarks, StdSort)(benchmark::State &state){
    for (auto _ : state) {

        // Re-shuffle array
        state.PauseTiming();
        std::shuffle(arr.begin(), arr.end(), std::mt19937(std::random_device()()));
        state.ResumeTiming();

        // Run the sorting algorithm
        std::sort(arr.begin(), arr.end());
    }

    state.SetComplexityN(state.range(0));
}

BENCHMARK_DEFINE_F(Benchmarks, IS4o)(benchmark::State &state){
    for (auto _ : state) {

        // Re-shuffle array
        state.PauseTiming();
        std::shuffle(arr.begin(), arr.end(), std::mt19937(std::random_device()()));
        state.ResumeTiming();

        // Run the sorting algorithm
        ips4o::sort(arr.begin(), arr.end());
    }

    state.SetComplexityN(state.range(0));
}


BENCHMARK_REGISTER_F(Benchmarks, StdSort)->RangeMultiplier(10)->Range(1E6, 1E8)->Unit(benchmark::kMillisecond)->Complexity(benchmark::BigO::oNLogN);
BENCHMARK_REGISTER_F(Benchmarks, IS4o)->RangeMultiplier(10)->Range(1E6, 1E8)->Unit(benchmark::kMillisecond)->Complexity(benchmark::BigO::oNLogN);

// Run the benchmark
BENCHMARK_MAIN();