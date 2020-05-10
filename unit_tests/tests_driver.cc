/**
 * @file tests_driver.cc
 * @author Ani Kristo (anikristo@gmail.com)
 * @brief Unit test driver file
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

#include <iostream>
#include <string>

#include "gtest/gtest.h"

using namespace std;

/**
 * @brief Global variable dictating the size of the tests (i.e., the number of
 * elements in the random arrays). Below is the default test size.
 */
size_t TEST_SIZE = 12345678;

int main(int argc, char **argv) {
  // Parse command line arguments
  ::testing::InitGoogleTest(&argc, argv);

  for (int i = 0; i < argc; i++) {
    static const string TEST_SIZE_OPTION_NAME = "--test_size";

    // Check for the test size option
    auto cur_opt = std::string(argv[i]);
    transform(cur_opt.begin(), cur_opt.end(), cur_opt.begin(), ::tolower);

    if (cur_opt.starts_with(TEST_SIZE_OPTION_NAME)) {
      TEST_SIZE = stoi(cur_opt.substr(TEST_SIZE_OPTION_NAME.length() + 1));
    }
  }

  return RUN_ALL_TESTS();
}