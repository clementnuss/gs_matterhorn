#include "gtest/gtest.h"

using namespace std;

TEST(LoggingTests, fileLoggerIsCorrect) {
    /*
    TelemetryReading t1{
            -1,
            0,
            0,
            0,
            0,
            0,
            {0, 0, 0}
    };

    TelemetryReading t2 = {
            -1,
            1.12456,
            2.12456,
            3.12456,
            4.12456,
            5.12456,
            {6.12456, 7.12456, 8.12456}
    };

    FileLogger fl{"testfile.csv"};
    vector<reference_wrapper<ILoggable>> v;

    v.emplace_back(reference_wrapper<ILoggable>{t1});

    // Put 2999 rows more for a total of 3000
    for (int i = 0; i < 3 * FileLogger::bufferSize - 1; i++) {
        v.emplace_back(reference_wrapper<ILoggable>{t2});
    }

    fl.registerData(v);
    fl.close();
    while (!fl.isReady()) {}

    ifstream input1("0testfile.csv");
    ifstream input2("1testfile.csv");
    ifstream input3("2testfile.csv");

    EXPECT_TRUE(input1);
    EXPECT_TRUE(input2);
    EXPECT_TRUE(input3);

    std::string strInput;

    getline(input1, strInput);
    cout << strInput << endl << flush;
    EXPECT_EQ(strInput, "-1\t0\t0\t0\t0\t0\t0\t0\t0");
    getline(input2, strInput);
    EXPECT_EQ(strInput, "-1\t1.12456\t2.12456\t3.12456\t4.12456\t5.12456\t6.12456\t7.12456\t8.12456");
    getline(input3, strInput);
    EXPECT_EQ(strInput, "-1\t1.12456\t2.12456\t3.12456\t4.12456\t5.12456\t6.12456\t7.12456\t8.12456");
    //TODO: check last line
    */
}