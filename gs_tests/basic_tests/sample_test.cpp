#include <General.h>
#include "gtest/gtest.h"


TEST(SampleTest, function_works_nominally) {
    EXPECT_EQ(14, simple_test_function(0));
}

TEST(SampleTest, test_error) {
    EXPECT_LT(13, simple_test_function(0xffffffff));
}
