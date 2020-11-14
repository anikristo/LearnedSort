/**
 * @file learned_sort_tests.cc
 * @author Ani Kristo (anikristo@gmail.com)
 * @brief Unit test files for Learned Sort
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

#include <algorithm>
#include <random>
#include <vector>

#include "../util.h"
#include "gtest/gtest.h"
#include "learned_sort.h"

using namespace std;

extern size_t TEST_SIZE;

TEST(LS_SORT_TEST, StdNormal) {
  // Generate random input
  auto arr = normal_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, Lognormal) {
  // Generate random input
  auto arr = lognormal_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, UniformReal) {
  // Generate random input
  auto arr = uniform_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, UniformInt) {
  // Generate random input
  auto arr = uniform_distr<int>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, UniformLong) {
  // Generate random input
  auto arr = uniform_distr<long>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, UniformUnsignedLong) {
  // Generate random input
  auto arr = uniform_distr<unsigned long>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, RootDups) {
  // Generate random input
  auto arr = root_dups_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, TwoDups) {
  // Generate random input
  auto arr = two_dups_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, EightDups) {
  // Generate random input
  auto arr = eight_dups_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, Sorted) {
  // Generate random input
  auto arr = sorted_uniform_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, ReverseSorted) {
  // Generate random input
  auto arr = reverse_sorted_uniform_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

/**
 * Test Case suggested by @Morwenn: https://github.com/Morwenn
 */
TEST(LS_SORT_TEST, UniformMod16) {
  // Generate random array
  std::vector<int> arr;
  std::mt19937_64 prng(1604922353);
  for (size_t i = 0; i < TEST_SIZE; ++i) {
    arr.emplace_back(i % 16);
  }
  std::shuffle(arr.begin(), arr.end(), prng);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, Zipf) {
  // Generate random input
  auto arr = zipf_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(LS_SORT_TEST, MixGauss) {
  // Generate random input
  auto arr = mix_of_gauss_distr<double>(TEST_SIZE);

  // Calculate the checksum
  auto cksm = get_checksum(arr);

  // Sort
  learned_sort::sort(arr.begin(), arr.end());

  // Test that the checksum is the same
  ASSERT_EQ(cksm, get_checksum(arr));

  // Test that it is sorted
  ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}
