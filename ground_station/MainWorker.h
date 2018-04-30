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
#include <UI/UIWidget.h>
#include "UI/GraphFeature.h"
#include "Loggers/FileLogger.h"
#include "Flyable.h"

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

    void transmitCommand(int);

signals:

    void loggingStatusReady(bool);

    void sensorsDataReady(SensorsPacket, FlyableType);

    void eventDataReady(EventPacket);

    void gpsDataReady(GPSPacket, FlyableType);

    void flightPositionReady(Position);

    void payloadPositionReady(Position);

    void graphDataReady(QVector<QCPGraphData> &, GraphFeature);

    void linkStatusReady(HandlerStatus);

    void groundStatusReady(float, float);

    void resetUIState();


private:

    void checkLinkStatuses();

    void processDataFlows();

    void logData();

    void fusionData();

    void displaySensorData(SensorsPacket &);

    void displayEventData(EventPacket &);

    void displayGPSData(GPSPacket &, FlyableType t);

    bool trackingEnabled_{false};
    bool loggingEnabled_;
    bool replayMode_;
    std::atomic<bool> updateHandler_;

#if USE_TRACKING
    boost::circular_buffer<double> altitudeBuffer_{25};
    chrono::system_clock::time_point lastTrackingAngleUpdate_;
    serial::Serial serialPort_{};
#endif


    unique_ptr<TelemetryHandler> telemetryHandler900MHz_;
    unique_ptr<TelemetryHandler> telemetryHandler433MHz_;
    unique_ptr<TelemetryHandler> newHandler_;
    FileLogger sensorsLogger900_;
    FileLogger eventsLogger900_;
    FileLogger gpsLogger900_;
    FileLogger sensorsLogger433_;
    FileLogger eventsLogger433_;
    FileLogger gpsLogger433_;
    uint32_t lastEventTimestamp_;
    uint32_t lastGPSTimestamp_;
    uint32_t lastPayloadGPSTimestamp_;
    Position lastComputedPosition_;
    Position lastComputedPayloadPosition_;
    chrono::system_clock::time_point lastNumericalValuesUpdate_;
    chrono::system_clock::time_point lastIteration_;
    chrono::system_clock::time_point timeOfLastLinkCheck_;
    chrono::system_clock::time_point timeOfLastReceivedTelemetry_;
    long long int millisBetweenLastTwoPackets_;

    QVector<QCPGraphData> extractGraphData(vector<SensorsPacket> &, QCPGraphData (*)(SensorsPacket));

    void moveTrackingSystem(double currentAltitude);
};

#endif // WORKER_H
