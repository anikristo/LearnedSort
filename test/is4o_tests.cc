#include <algorithm>
#include <random>
#include <vector>
#include "gtest/gtest.h"
#include "ips4o.hpp"
#include "util.hh"

using namespace std;

namespace {

    class IS4o_TEST : public ::testing::Test {

    public:

        /// Constants ///
        const int NUM_RECORDS = 2e7;

        const float NORMAL_MEAN = 0;
        const float NORMAL_STDEV = 1;

        const float LOGNORMAL_MEAN = 0;
        const float LOGNORMAL_STDEV = 2;

        const float UNIFORM_MIN = -100;
        const float UNIFORM_MAX = 500;

    };

    TEST_F(IS4o_TEST, StdNormal) {

        // Generate random input
        vector<double> arr;
        random_device rd;
        mt19937 generator(rd());
        normal_distribution<> distribution(NORMAL_MEAN, NORMAL_STDEV);
        for (int i = 0; i < NUM_RECORDS; i++) {
            arr.push_back(distribution(generator));
        }

        // Calculate the checksum
        auto cksm = get_checksum(arr);

        // Sort
        ips4o::sort(arr.begin(), arr.end());

        // Test that it is sorted
        ASSERT_TRUE (std::is_sorted(arr.begin(), arr.end()));

        // Test that the checksum is the same
        ASSERT_EQ(cksm, get_checksum(arr));
    }

    TEST_F(IS4o_TEST, Lognormal) {

        // Generate random input
        vector<double> arr;
        random_device rd;
        mt19937 generator(rd());
        lognormal_distribution<> distribution(LOGNORMAL_MEAN, LOGNORMAL_STDEV);
        for (int i = 0; i < NUM_RECORDS; i++) {
            arr.push_back(distribution(generator));
        }

        // Calculate the checksum
        auto cksm = get_checksum(arr);

        // Sort
        ips4o::sort(arr.begin(), arr.end());

        // Test that it is sorted
        ASSERT_TRUE (std::is_sorted(arr.begin(), arr.end()));

        // Test that the checksum is the same
        ASSERT_EQ(cksm, get_checksum(arr));
    }

    TEST_F(IS4o_TEST, UniformReal) {

        // Generate random input
        vector<double> arr;
        random_device rd;
        mt19937 generator(rd());
        uniform_real_distribution<> distribution(UNIFORM_MIN, UNIFORM_MAX);
        for (int i = 0; i < NUM_RECORDS; i++) {
            arr.push_back(distribution(generator));
        }

        // Calculate the checksum
        auto cksm = get_checksum(arr);

        // Sort
        ips4o::sort(arr.begin(), arr.end());

        // Test that it is sorted
        ASSERT_TRUE (std::is_sorted(arr.begin(), arr.end()));

        // Test that the checksum is the same
        ASSERT_EQ(cksm, get_checksum(arr));
    }

    TEST_F(IS4o_TEST, UniformInt) {

        // Generate random input
        vector<int> arr;
        random_device rd;
        mt19937 generator(rd());
        uniform_int_distribution<> distribution(UNIFORM_MIN, UNIFORM_MAX);
        for (int i = 0; i < NUM_RECORDS; i++) {
            arr.push_back(distribution(generator));
        }

        // Calculate the checksum
        auto cksm = get_checksum(arr);

        // Sort
        ips4o::sort(arr.begin(), arr.end());

        // Test that it is sorted
        ASSERT_TRUE (std::is_sorted(arr.begin(), arr.end()));

        // Test that the checksum is the same
        ASSERT_EQ(cksm, get_checksum(arr));
    }
}