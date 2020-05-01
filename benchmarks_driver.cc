#include <algorithm>
#include <benchmark/benchmark.h>
#include <random>

#include "gfx/timsort.hpp"
#include "ips4o.hpp"
#include "src/learned_sort.hh"
#include "radix_sort.hh"

using namespace std;

class Benchmarks : public benchmark::Fixture
{
protected:
  void SetUp(const ::benchmark::State &state)
  {

    // Initialize random engine with normal distribution
    random_device rd;
    mt19937 generator(rd());
    normal_distribution<> distribution(0, 1);

    // Populate the input
    const auto INPUT_SZ = state.range(0);
    for (int i = 0; i < INPUT_SZ; i++)
    {
      arr.push_back(distribution(generator));
    }
  }

  void TearDown(const ::benchmark::State &state)
  {
    // Verify that the array is sorted
    if (!is_sorted(arr.begin(), arr.end()))
    {
      cerr << "Array not sorted! Exiting." << endl;
      exit(EXIT_FAILURE);
    }

    arr.clear();
  }

  // Input array
  vector<double> arr;
};

BENCHMARK_DEFINE_F(Benchmarks, LearnedSort)
(benchmark::State &state)
{
  for (auto _ : state)
  {
    // Re-shuffle
    state.PauseTiming();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(arr.begin(), arr.end(), g);
    state.ResumeTiming();

    // Sort
    learned_sort::sort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_DEFINE_F(Benchmarks, StdSort)
(benchmark::State &state)
{
  for (auto _ : state)
  {
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
(benchmark::State &state)
{
  for (auto _ : state)
  {
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

BENCHMARK_DEFINE_F(Benchmarks, RadixSort)
(benchmark::State &state)
{
  for (auto _ : state)
  {
    // Re-shuffle
    state.PauseTiming();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(arr.begin(), arr.end(), g);
    state.ResumeTiming();

    // Sort
    radix_sort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_DEFINE_F(Benchmarks, Timsort)
(benchmark::State &state)
{
  for (auto _ : state)
  {
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

static void benchmark_arguments(benchmark::internal::Benchmark *b)
{
  // Set input size parameter
  b->Arg(1E6); // 1M
  b->Arg(1E7); // 10M
  b->Arg(1E8); // 100M
  b->Arg(1E9); // 1B

  // Set time measurement unit
  b->Unit(benchmark::kMillisecond);
}

BENCHMARK_REGISTER_F(Benchmarks, LearnedSort)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oAuto);
BENCHMARK_REGISTER_F(Benchmarks, IS4o)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);
BENCHMARK_REGISTER_F(Benchmarks, RadixSort)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oN);
BENCHMARK_REGISTER_F(Benchmarks, StdSort)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);
BENCHMARK_REGISTER_F(Benchmarks, Timsort)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);

// Run the benchmark
BENCHMARK_MAIN();