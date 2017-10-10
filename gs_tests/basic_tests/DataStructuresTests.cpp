
#include "gtest/gtest.h"
#include "DataStructures/datastructs.h"

TEST(DataStructuresTests, timedTelemetryDataIsCorrect) {

    TelemetryReading t1{
            -1,
            {0, true},
            {0, true},
            {0, true},
            {0, true},
            {0, true},
            {0, 0, 0, true}
    };

    TelemetryReading t2 = {
            -1,
            {1, true},
            {2, true},
            {3, true},
            {4, true},
            {5, true},
            {6, 7, 8, true}
    };

    EXPECT_EQ(std::string("-1\t0\t0\t0\t0\t0\t0\t0\t0"), t1.toString());
    EXPECT_EQ(std::string("-1\t1\t2\t3\t4\t5\t6\t7\t8"), t2.toString());
}

