
#include <QtWidgets/QLineEdit>
#include <DataStructures/Datastructs.h>
#include <Application.h>
#include "GsMainwindowTests.h"

void GsMainwindowTests::telemetryReadingCorrectlyDisplayed() {
    qRegisterMetaType<SensorsPacket>("TelemetryReading");
    qRegisterMetaType<QVector<QCPGraphData>>("QVector<QCPGraphData>&");
    qRegisterMetaType<QVector<QVector3D>>("QVector<QVector3D>&");
    qRegisterMetaType<vector<EventPacket>>
            ("vector<RocketEvent>&");
    qRegisterMetaType<GraphFeature>("GraphFeature");
    qRegisterMetaType<HandlerStatus>("HandlerStatus");

    QCOMPARE(gsMainWindow_.ui->ground_pressure_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->ground_temperature_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->telemetry_yaw_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->telemetry_pitch_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->telemetry_roll_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->telemetry_altitude_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->telemetry_acceleration_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->telemetry_pressure_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->telemetry_speed_value->text().toStdString(), "0.0");
    QCOMPARE(gsMainWindow_.ui->telemetry_temperature_value->text().toStdString(), "0.0");

    SensorsPacket r{1, 1234, FlyableType::ROCKET, 100.14, {1, 0, 0}, {1, 1, 1}, {1, 1, 1},
                    40.14, 25.086, 15.149};
    //gsMainWindow_.receiveSensorData(r);

    QCOMPARE(gsMainWindow_.ui->telemetry_yaw_value->text().toStdString(), "1.00");
    QCOMPARE(gsMainWindow_.ui->telemetry_pitch_value->text().toStdString(), "1.00");
    QCOMPARE(gsMainWindow_.ui->telemetry_roll_value->text().toStdString(), "1.00");
    QCOMPARE(gsMainWindow_.ui->telemetry_altitude_value->text().toStdString(), "100.14");
    QCOMPARE(gsMainWindow_.ui->telemetry_acceleration_value->text().toStdString(), "1.00");
    QCOMPARE(gsMainWindow_.ui->telemetry_pressure_value->text().toStdString(), "40.14");
    QCOMPARE(gsMainWindow_.ui->telemetry_speed_value->text().toStdString(), "15.15");
    QCOMPARE(gsMainWindow_.ui->telemetry_temperature_value->text().toStdString(), "25.09");
}

void GsMainwindowTests::correctSignalsActivation() {
    QSignalSpy loggingToggleSpy(&gsMainWindow_, &GSMainwindow::toggleLogging);
    QTest::keyEvent(QTest::KeyAction::Press, &gsMainWindow_, Qt::Key_Space);
    QCOMPARE(loggingToggleSpy.count(), 1);

    QSignalSpy resetPlaybackSpy(&gsMainWindow_, &GSMainwindow::resetTelemetryReplayPlayback);
    gsMainWindow_.setReplayMode();
    gsMainWindow_.resetPlayback();
    QVERIFY(resetPlaybackSpy.count() == 1);

    QSignalSpy playbackSpeedChangeSpy(&gsMainWindow_, &GSMainwindow::changePlaybackSpeed);
    gsMainWindow_.ui->time_unfolding_increase->click();
    auto args = playbackSpeedChangeSpy.takeFirst();
    QCOMPARE(args.at(0).toDouble(), 1.5);
    gsMainWindow_.ui->time_unfolding_decrease->click();
    args = playbackSpeedChangeSpy.takeFirst();
    QCOMPARE(args.at(0).toDouble(), 1);

    QSignalSpy reverseTelemetryPlaybackSpy(&gsMainWindow_, &GSMainwindow::reverseTelemetryReplayPlayback);
    gsMainWindow_.ui->time_unfolding_reverse_time->click();
    args = reverseTelemetryPlaybackSpy.takeFirst();
    QCOMPARE(args.at(0).toBool(), true);
}

void GsMainwindowTests::stackedWidgetsCorrectlyChanges() {
    auto previousIndex = gsMainWindow_.ui->stackedWidget->currentIndex();

    for (int i = 0; i < 2 * gsMainWindow_.ui->stackedWidget->count(); i++) {
        QTest::keyEvent(QTest::KeyAction::Press, &gsMainWindow_, Qt::Key_Control);
        previousIndex++;
        previousIndex %= gsMainWindow_.ui->stackedWidget->count();
        QCOMPARE(gsMainWindow_.ui->stackedWidget->currentIndex(), previousIndex);
    }
}