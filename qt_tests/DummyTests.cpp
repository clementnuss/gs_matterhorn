
#include "DummyTests.h"

QTEST_MAIN(DummyTests)

void DummyTests::testsAreCorrectlySetup() {
    QVERIFY(true);
}

void DummyTests::toUpper_data() {
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("all lower") << "hello" << "HELLO";
    QTest::newRow("mixed") << "Hello" << "HELLO";
    QTest::newRow("all upper") << "HELLO" << "HELLO";
}

void DummyTests::toUpper() {
    QFETCH(QString, string);
    QFETCH(QString, result);

    QCOMPARE(string.toUpper(), result);
}
