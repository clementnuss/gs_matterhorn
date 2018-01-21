#ifndef GS_MATTERHORN_DUMMYTESTS_H
#define GS_MATTERHORN_DUMMYTESTS_H


#include <QtTest/QtTest>
#include <QtWidgets/QLineEdit>

class DummyTests : public QObject {
Q_OBJECT
private slots:

    void testsAreCorrectlySetup() {
        QVERIFY(true);
    }

    void toUpper_data() {
        QTest::addColumn<QString>("string");
        QTest::addColumn<QString>("result");

        QTest::newRow("all lower") << "hello" << "HELLO";
        QTest::newRow("mixed") << "Hello" << "HELLO";
        QTest::newRow("all upper") << "HELLO" << "HELLO";
    }

    void toUpper() {
        QFETCH(QString, string);
        QFETCH(QString, result);

        QCOMPARE(string.toUpper(), result);
    }

    void testGui_data() {
        QTest::addColumn<QTestEventList>("events");
        QTest::addColumn<QString>("expected");

        QTestEventList list1;
        list1.addKeyClick('a');
        QTest::newRow("char") << list1 << "a";

        QTestEventList list2;
        list2.addKeyClick('a');
        list2.addKeyClick(Qt::Key_Backspace);
        QTest::newRow("there and back again") << list2 << "";
    }

    void testGui() {
        QFETCH(QTestEventList, events);
        QFETCH(QString, expected);

        QLineEdit lineEdit;

        events.simulate(&lineEdit);

        QCOMPARE(lineEdit.text(), expected);
    }
};


#endif //GS_MATTERHORN_DUMMYTESTS_H
