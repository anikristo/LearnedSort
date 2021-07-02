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
#include "learned_sort.h"
#include "pdqsort.h"
#include "radix_sort.h"
#include "ska_sort.hpp"
#include "utils.h"

using namespace std;

// NOTE: You may change the data type here
typedef unsigned long data_t;

// NOTE: You may change the dataset here
// For a list of real datasets see the `data/` directory.
// Make sure to specify the correct data type for the selected dataset.
const string DATASET = "OSM/Cell_IDs";

constexpr size_t REPS = 5;

static void benchmark_arguments(benchmark::internal::Benchmark *b) {
  b->Unit(benchmark::kMillisecond);
  b->Repetitions(REPS);
}

static bool size_displayed = false;
class Benchmarks : public benchmark::Fixture {
 protected:
  void SetUp(const ::benchmark::State &state) {
    // Read the input file
    std::ifstream ifs("data/" + DATASET + ".txt");
    if (ifs.fail()) {
      cerr << "Cannot open data file: " << strerror(errno) << endl;
    }
    std::istream_iterator<data_t> start(ifs), end;
    std::copy(start, end, std::back_inserter(arr));

    // Calculate the checksum
    cksm = get_checksum(arr);

    // Display dataset size
    if (!size_displayed) {
      cout << "Dataset: " << DATASET << endl;
      cout << arr.size() << " keys to sort." << endl;
      size_displayed = true;
    }
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
    // Clenaup
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
                      learned_sort::sort(arr.begin(), arr.end()))
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
