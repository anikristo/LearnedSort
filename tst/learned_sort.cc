#include "gtest/gtest.h"
#include "learned_sort.h"

TEST(LS_TEST, Uniform) {
    EXPECT_NO_THROW (learned_sort::sort());
}