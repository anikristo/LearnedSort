#include <algorithm>
#include <benchmark/benchmark.h>
#include <random>

#include "gfx/timsort.hpp"
#include "ips4o.hpp"

using namespace std;

class Benchmarks : public benchmark::Fixture {
protected:
  void SetUp(const ::benchmark::State &state) {

    // Initialize random engine with normal distribution
    random_device rd;
    mt19937 generator(rd());
    normal_distribution<> distribution(0, 1);

    // Populate the input
    for (int i = 0; i < state.range(0); i++) {
      arr.push_back(distribution(generator));
    }
  }

  void TearDown(const ::benchmark::State &state) {
    // Verify that the array is sorted
    if (!is_sorted(arr.begin(), arr.end())) {
      cout << "A" << endl;
      cerr << "Array not sorted! Exiting." << endl;
      exit(EXIT_FAILURE);
    }
  }

  // Input array
  vector<double> arr;
};

BENCHMARK_DEFINE_F(Benchmarks, StdSort)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(arr.begin(), arr.end(), g);
    state.ResumeTiming();

    // Sort
    std::sort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_DEFINE_F(Benchmarks, IS4o)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(arr.begin(), arr.end(), g);
    state.ResumeTiming();

    // Sort
    ips4o::sort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_DEFINE_F(Benchmarks, Timsort)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(arr.begin(), arr.end(), g);
    state.ResumeTiming();

    // Sort
    gfx::timsort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

static void benchmark_arguments(benchmark::internal::Benchmark *b) {
  // b->Arg(1E5)->Arg(1E6)->Arg(1E7)->Arg(1E8)->Arg(1E9);
  b->Arg(1E5)->Arg(1E6)->Arg(1E7)->Arg(1E8);
  b->Unit(benchmark::kMillisecond);
}

BENCHMARK_REGISTER_F(Benchmarks, StdSort)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);
BENCHMARK_REGISTER_F(Benchmarks, IS4o)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);
BENCHMARK_REGISTER_F(Benchmarks, Timsort)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);

// Run the benchmark
BENCHMARK_MAIN();