#ifndef GS_MATTERHORN_REPLAYTESTS_H
#define GS_MATTERHORN_REPLAYTESTS_H

#include <QtTest/QtTest>
#include "UIWidget.h"

class GsMainwindowTests : public QObject {
Q_OBJECT

private:
    GSMainwindow gsMainWindow_{};

private slots:

    void telemetryReadingCorrectlyDisplayed();
    void correctSignalsActivation();


};

#endif //GS_MATTERHORN_REPLAYTESTS_H
