// Source: https://gist.github.com/edwrodrig/5c84af39bc151f4cd815#file-qt_test_main_lambda-cpp
//

#include <3DTests/IntervalTests.h>
#include <3DTests/GroundTests.h>
#include <3DTests/ElevationModelTests.h>
#include "DummyTests.h"
#include "GsMainwindowTests.h"

int main(int argc, char **argv) {
    int status = 0;
    auto ASSERT_TEST = [&status, argc, argv](QObject *obj) {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    QApplication app(argc, argv);

    ASSERT_TEST(new GsMainwindowTests());
    ASSERT_TEST(new DummyTests());
    ASSERT_TEST(new IntervalTests());
    ASSERT_TEST(new GroundTests());
    ASSERT_TEST(new ElevationModelTests());

    return status;
}