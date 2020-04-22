#include <iostream>
#include <string>
#include "gtest/gtest.h"

using namespace std;

/**
 * @brief Global variable dictating the size of the tests (i.e., the number of elements in the random arrays). 20M is the default test size.
 */
size_t TEST_SIZE = 2e7; 

int main(int argc, char **argv)
{

  // Parse command line arguments
  ::testing::InitGoogleTest(&argc, argv);

  for (int i = 0; i < argc; i++)
  {

    static const string TEST_SIZE_OPTION_NAME = "--test_size";

    // Check for the test size option
    auto cur_opt = std::string(argv[i]);
    transform(cur_opt.begin(), cur_opt.end(), cur_opt.begin(), ::tolower);

    if (cur_opt.starts_with(TEST_SIZE_OPTION_NAME))
    {
      TEST_SIZE = stoi(cur_opt.substr(TEST_SIZE_OPTION_NAME.length() + 1));
    }
  }

  return RUN_ALL_TESTS();
}