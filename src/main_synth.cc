/**
 * @author Ani Kristo (anikristo@gmail.com)
 * @brief Driver file for the performance benchmarks
 *
 * @copyright Copyright (c) 2021 Ani Kristo (anikristo@gmail.com)
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
#include "pdqsort.h"
#include "learned_sort.h"
#include "radix_sort.h"
#include "ska_sort.hpp"
#include "utils.h"

using namespace std;

// NOTE: You may change the data type here
typedef double data_t;

// NOTE: You may change the distribution here.
// For a list of supported distributions see src/utils.h
distr_t DATA_DISTR = NORMAL;

// NOTE: You may change the input size here
constexpr size_t INPUT_SZ = 50'000'000;

constexpr size_t REP_LARGE_INPUTS = 5;
constexpr size_t REP_SMALL_INPUTS = 10;

static void benchmark_arguments(benchmark::internal::Benchmark *b) {
  b->Arg(INPUT_SZ);
  b->Unit(benchmark::kMillisecond);
}

class Benchmarks : public benchmark::Fixture {
 public:
  Benchmarks() {
    if (INPUT_SZ < 1e8)
      Repetitions(REP_SMALL_INPUTS);
    else
      Repetitions(REP_LARGE_INPUTS);
  }

 protected:
  void SetUp(const ::benchmark::State &state) {
    // Generate the synthetic data
    size_t size = state.range(0);
    arr = generate_data<data_t>(DATA_DISTR, size);

    // Calculate the checksum
    cksm = get_checksum(arr);
  }

  void TearDown(const ::benchmark::State &state) {
    // Verify that the array's checksum is correct
    if (get_checksum(arr) != cksm) {
      cerr << "Incorrect checksum! Exiting." << endl;
      exit(EXIT_FAILURE);
    }

    // Verify that the array is sorted
    for (size_t i = 1; i < arr.size(); i++) {
      if (arr[i] < arr[i - 1]) {
        if (i == arr.size() - 1)
          cout << "Unsorted elements in position " << i << ": ..." << arr[i - 1]
               << ", " << arr[i] << ".\n";
        else
          cout << "Unsorted elements in position " << i << ": ..." << arr[i - 1]
               << ", " << arr[i] << ", " << arr[i + 1] << "...\n";
        exit(EXIT_FAILURE);
      }
    }

    // Cleanup
    arr.clear();
  }

  // Input array
  vector<data_t> arr;

  // Checksum
  long long cksm;
};

#define SORT_BENCHMARK_DEFINE(SortFnName, SortFnCall) \
  BENCHMARK_DEFINE_F(Benchmarks, SortFnName)          \
  (benchmark::State & state) {                        \
    for (auto _ : state) {                            \
      SortFnCall;                                     \
    }                                                 \
  }                                                   \
  BENCHMARK_REGISTER_F(Benchmarks, SortFnName)->Apply(benchmark_arguments);

// Register the benchmarks
SORT_BENCHMARK_DEFINE(LearnedSort,
                      learned_sort::sort(arr.begin(),
                      arr.end()))
SORT_BENCHMARK_DEFINE(RadixSort, radix_sort(arr.begin(), arr.end()))
SORT_BENCHMARK_DEFINE(IS4o, ips4o::sort(arr.begin(), arr.end()))
SORT_BENCHMARK_DEFINE(StdSort, std::sort(arr.begin(), arr.end()))
SORT_BENCHMARK_DEFINE(BlockQuicksort,
                      blocked_double_pivot_check_mosqrt::sort(arr))
SORT_BENCHMARK_DEFINE(SkaSort, ska_sort(arr.begin(), arr.end()))
SORT_BENCHMARK_DEFINE(Timsort, gfx::timsort(arr.begin(), arr.end()))
SORT_BENCHMARK_DEFINE(PDQS, pdqsort(arr.begin(), arr.end()))

// Run the benchmark
BENCHMARK_MAIN();
