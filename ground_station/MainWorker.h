#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <DataHandlers/TelemetryHandler.h>
#include <qcustomplot.h>
#include <chrono>
#include <serial/serial.h>
#include <boost/circular_buffer.hpp>
#include <3D/CoordinateUtils.h>
#include "UI/GraphFeature.h"
#include "Loggers/FileLogger.h"

class GSMainwindow;
using namespace std;

class Worker : public QObject {
Q_OBJECT

public:
    explicit Worker(GSMainwindow *);

    ~Worker() override;

    void mainRoutine();

    void defineReplayMode(const QString &);

    void defineRealtimeMode(const QString &parameters);


public slots:

    void run();

    void emitAllStatuses();

    void updateLoggingStatus();

    void toggleTracking();

    void updatePlaybackSpeed(double);

    void resetPlayback();

    void reversePlayback(bool);

signals:

    void loggingStatusReady(bool);

    void sensorsDataReady(SensorsPacket);

    void eventDataReady(EventPacket);

    void gpsDataReady(GPSPacket);

    void flightPositionReady(Position);

    void graphDataReady(QVector<QCPGraphData> &, GraphFeature);

    void linkStatusReady(HandlerStatus);

    void groundStatusReady(float, float);

    void resetUIState();


private:

    void checkLinkStatuses();

    void displaySensorData(SensorsPacket&);

    void displayEventData(EventPacket&);

    void displayGPSData(GPSPacket&);

    bool trackingEnabled_{false};
    bool loggingEnabled_;
    bool replayMode_;
    std::atomic<bool> updateHandler_;

#if USE_TRACKING
    boost::circular_buffer<double> altitudeBuffer_{25};
    chrono::system_clock::time_point lastTrackingAngleUpdate_;
    serial::Serial serialPort_{};
#endif


    unique_ptr<TelemetryHandler> telemetryHandler_;
    unique_ptr<TelemetryHandler> newHandler_;
    FileLogger sensorsLogger_;
    FileLogger eventsLogger_;
    FileLogger gpsLogger_;
    uint32_t lastEventTimestamp_;
    uint32_t lastGPSTimestamp_;
    Position lastComputedPosition_;
    chrono::system_clock::time_point lastNumericalValuesUpdate_;
    chrono::system_clock::time_point lastIteration_;
    chrono::system_clock::time_point timeOfLastLinkCheck_;
    chrono::system_clock::time_point timeOfLastReceivedTelemetry_;
    long long int millisBetweenLastTwoPackets_;

    QVector<QCPGraphData> extractGraphData(vector<SensorsPacket> &, QCPGraphData (*)(SensorsPacket));

    void moveTrackingSystem(double currentAltitude);
};

#endif // WORKER_H
