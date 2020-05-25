/**
 * @file benchmarks_driver.cc
 * @author Ani Kristo (anikristo@gmail.com)
 * @brief Driver file for the performance benchmarks
 *
 * @copyright Copyright (c) 2020 Ani Kristo (anikristo@gmail.com)
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <benchmark/benchmark.h>

#include <algorithm>

#include "gfx/timsort.hpp"
#include "ips4o.hpp"
#include "learned_sort.h"
#include "radix_sort.hh"
#include "util.h"

using namespace std;

distr_t DATA_DISTR = NORMAL;  // NOTE: You can change the distribution here
typedef double data_t;

class Benchmarks : public benchmark::Fixture {
 protected:
  void SetUp(const ::benchmark::State &state) {
    size_t size = state.range(0);

    switch (DATA_DISTR) {
      case NORMAL:
        arr = normal_distr<data_t>(size);

      case UNIFORM:
        arr = uniform_distr<data_t>(size);

      case EXPONENTIAL:
        arr = exponential_distr<data_t>(size);

      case LOGNORMAL:
        arr = lognormal_distr<data_t>(size);

      default:
        arr = normal_distr<data_t>(size);
    }
  }

  void TearDown(const ::benchmark::State &state) {
    // Verify that the array is sorted
    if (!is_sorted(arr.begin(), arr.end())) {
      cerr << "Array not sorted! Exiting." << endl;
      exit(EXIT_FAILURE);
    }

    arr.clear();
  }

  // Input array
  vector<data_t> arr;
};

BENCHMARK_DEFINE_F(Benchmarks, LearnedSort)
(benchmark::State &state) {
  for (auto _ : state) {
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

BENCHMARK_DEFINE_F(Benchmarks, RadixSort)
(benchmark::State &state) {
  for (auto _ : state) {
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
  // Set input size parameter
  b->Arg(1E6);  // 1M
  b->Arg(1E7);  // 10M
  b->Arg(1E8);  // 100M
  b->Arg(1E9);  // 1B

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