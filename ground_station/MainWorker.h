#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QtWidgets/QWidget>
#include <DataStructures/datastructs.h>
#include <DataHandlers/TelemetryHandler.h>
#include <qcustomplot.h>
#include <chrono>
#include "GraphFeature.h"
#include "FileLogger.h"
#include <serial/serial.h>
#include <boost/circular_buffer.hpp>

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

    void telemetryReady(TelemetryReading);

    void graphDataReady(QVector<QCPGraphData> &, GraphFeature);

    void newEventsReady(vector<RocketEvent> &);

    void status3DReady(QVector<QVector3D> &, QVector3D &);

    void linkStatusReady(HandlerStatus);

    void groundStatusReady(float, float);

    void resetUIState();


private:
    double lastAltitude;

    bool trackingEnabled_{false};
    bool loggingEnabled_;
    bool replayMode_;
    std::atomic<bool> updateHandler_;

#if USE_TRACKING
    boost::circular_buffer<int> angleBuffer_{25};
    chrono::system_clock::time_point lastTrackingAngleUpdate;
    serial::Serial serialPort_{};
#endif

    void checkLinkStatuses();

    void displayMostRecentTelemetry(TelemetryReading);


    unique_ptr<TelemetryHandler> telemetryHandler_;
    unique_ptr<TelemetryHandler> newHandler_;
    FileLogger telemetryLogger;
    FileLogger eventLogger;
    chrono::system_clock::time_point lastUIupdate;
    chrono::system_clock::time_point lastIteration;
    chrono::system_clock::time_point timeOfLastLinkCheck;
    chrono::system_clock::time_point timeOfLastReceivedTelemetry;
    long long int millisBetweenLastTwoPackets;

    QVector<QCPGraphData> extractGraphData(vector<TelemetryReading> &, QCPGraphData (*)(TelemetryReading));

    void moveTrackingSystem(double currentAltitude);
};

#endif // WORKER_H
