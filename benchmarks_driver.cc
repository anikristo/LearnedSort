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
#include "radix_sort.hh"
#include "ska_sort.hpp"
#include "util.h"

using namespace std;

// NOTE: You can change the distribution here
distr_t DATA_DISTR = NORMAL;
typedef double data_t;

class Benchmarks : public benchmark::Fixture {
 protected:
  void SetUp(const ::benchmark::State &state) {
    size_t size = state.range(0);

    switch (DATA_DISTR) {
      case EIGHT_DUPS:
        arr = eight_dups_distr<data_t>(size);
        break;

      case EXPONENTIAL:
        arr = exponential_distr<data_t>(size);
        break;

      case LOGNORMAL:
        arr = lognormal_distr<data_t>(size);
        break;

      case MIX_OF_GAUSS:
        arr = mix_of_gauss_distr<data_t>(size);
        break;

      case NORMAL:
        arr = normal_distr<data_t>(size);
        break;

      case REVERSE_SORTED_UNIFORM:
        arr = reverse_sorted_uniform_distr<data_t>(size);
        break;

      case ROOT_DUPS:
        arr = root_dups_distr<data_t>(size);
        break;

      case SORTED_UNIFORM:
        arr = sorted_uniform_distr<data_t>(size);
        break;

      case TWO_DUPS:
        arr = two_dups_distr<data_t>(size);
        break;

      case UNIFORM:
        arr = uniform_distr<data_t>(size);
        break;

      case ZIPF:
        arr = zipf_distr<data_t>(size);
        break;

      default:
        arr = normal_distr<data_t>(size);
        break;
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
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
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
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
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
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    ips4o::sort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

// NOTE:
// The provided implementation for Radix Sort only works with vector<double>
BENCHMARK_DEFINE_F(Benchmarks, RadixSort)(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
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
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    gfx::timsort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_DEFINE_F(Benchmarks, BlockQuicksort)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    blocked_double_pivot_check_mosqrt::sort(arr);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_DEFINE_F(Benchmarks, PDQS)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    pdqsort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK_DEFINE_F(Benchmarks, SkaSort)
(benchmark::State &state) {
  for (auto _ : state) {
    // Re-shuffle
    state.PauseTiming();
    if (DATA_DISTR != SORTED_UNIFORM && DATA_DISTR != REVERSE_SORTED_UNIFORM) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(arr.begin(), arr.end(), g);
    }
    state.ResumeTiming();

    // Sort
    ska_sort(arr.begin(), arr.end());
  }

  state.SetComplexityN(state.range(0));
}

static void benchmark_arguments(benchmark::internal::Benchmark *b) {
  // Set input size parameter
  b->Arg(10'000'000);  // 10M Keys

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
BENCHMARK_REGISTER_F(Benchmarks, BlockQuicksort)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);
BENCHMARK_REGISTER_F(Benchmarks, PDQS)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);
BENCHMARK_REGISTER_F(Benchmarks, SkaSort)
    ->Apply(benchmark_arguments)
    ->Complexity(benchmark::BigO::oNLogN);

// Run the benchmark
BENCHMARK_MAIN();
