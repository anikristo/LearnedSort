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

#include "blocked_double_pivot_check_mosqrt.h++"
#include "gfx/timsort.hpp"
#include "ips4o.hpp"
#include "learned_sort.h"
#include "pdqsort.h"
#include "radix_sort.h"
#include "ska_sort.hpp"
#include "utils.h"

using namespace std;

// NOTE: You can change the distribution here
distr_t DATA_DISTR = NORMAL;
typedef double data_t;

class Benchmarks : public benchmark::Fixture {
 protected:
  void SetUp(const ::benchmark::State &state) {
    size_t size = state.range(0);
    arr = generate_data<data_t>(DATA_DISTR, size);
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
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM &&
        DATA_DISTR != IDENTICAL) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    learned_sort::sort(arr.begin(), arr.end());
  }
}

BENCHMARK_DEFINE_F(Benchmarks, StdSort)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM &&
        DATA_DISTR != IDENTICAL) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    std::sort(arr.begin(), arr.end());
  }
}

BENCHMARK_DEFINE_F(Benchmarks, IS4o)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM &&
        DATA_DISTR != IDENTICAL) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    ips4o::sort(arr.begin(), arr.end());
  }
}

// NOTE:
// The provided implementation for Radix Sort only works with vector<double>
BENCHMARK_DEFINE_F(Benchmarks, RadixSort)(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM &&
        DATA_DISTR != IDENTICAL) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    radix_sort(arr.begin(), arr.end());
  }
}

BENCHMARK_DEFINE_F(Benchmarks, Timsort)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM &&
        DATA_DISTR != IDENTICAL) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    gfx::timsort(arr.begin(), arr.end());
  }
}

BENCHMARK_DEFINE_F(Benchmarks, BlockQuicksort)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM &&
        DATA_DISTR != IDENTICAL) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    blocked_double_pivot_check_mosqrt::sort(arr);
  }
}

BENCHMARK_DEFINE_F(Benchmarks, PDQS)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM &&
        DATA_DISTR != IDENTICAL) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    pdqsort(arr.begin(), arr.end());
  }
}

BENCHMARK_DEFINE_F(Benchmarks, SkaSort)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM &&
        DATA_DISTR != IDENTICAL) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    ska_sort(arr.begin(), arr.end());
  }
}

static void benchmark_arguments(benchmark::internal::Benchmark *b) {
  // Set input size parameter
  b->Arg(10'000'000);  // 10M Keys

  // Set time measurement unit
  b->Unit(benchmark::kMillisecond);
}

BENCHMARK_REGISTER_F(Benchmarks, LearnedSort)->Apply(benchmark_arguments);
BENCHMARK_REGISTER_F(Benchmarks, IS4o)->Apply(benchmark_arguments);
BENCHMARK_REGISTER_F(Benchmarks, RadixSort)->Apply(benchmark_arguments);
BENCHMARK_REGISTER_F(Benchmarks, StdSort)->Apply(benchmark_arguments);
BENCHMARK_REGISTER_F(Benchmarks, Timsort)->Apply(benchmark_arguments);
BENCHMARK_REGISTER_F(Benchmarks, BlockQuicksort)->Apply(benchmark_arguments);
BENCHMARK_REGISTER_F(Benchmarks, PDQS)->Apply(benchmark_arguments);
BENCHMARK_REGISTER_F(Benchmarks, SkaSort)->Apply(benchmark_arguments);

// Run the benchmark
BENCHMARK_MAIN();
