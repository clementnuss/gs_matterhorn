#ifndef GS_MATTERHORN_DUMMYTESTS_H
#define GS_MATTERHORN_DUMMYTESTS_H


#include <QtTest/QtTest>

class DummyTests : public QObject {
Q_OBJECT
private slots:

    void testsAreCorrectlySetup();

    void toUpper_data();

    void toUpper();

    void testGui_data();

    void testGui();
};


#endif //GS_MATTERHORN_DUMMYTESTS_H
