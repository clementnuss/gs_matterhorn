#include <3DTests/IntervalTests.h>
#include <3DTests/ElevationModelTests.h>
#include <3DTests/GroundTests.h>
#include "DummyTests.h"

int main(int argc, char **argv) {

    QApplication app{argc, argv};
    QTest::qExec(new DummyTests());
    QTest::qExec(new IntervalTests());
    QTest::qExec(new ElevationModelTests());
    QTest::qExec(new GroundTests());

}
