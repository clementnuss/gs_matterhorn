#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QtWidgets/QWidget>
#include <DataStructures/Datastructs.h>
#include <DataHandlers/IReceiver.h>
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

    void dataPacketReady(SensorsPacket) const;

    void dataPacketReady(EventPacket) const;

    void dataPacketReady(GPSPacket) const;

    void flightPositionReady(Position);

    void payloadPositionReady(Position);

    void graphDataReady(QVector<QCPGraphData> &, GraphFeature);

    void linkStatusReady(HandlerStatus);

    void groundStatusReady(float, float);

    void resetUIState();


private:

    void checkLinkStatuses();

    PacketDispatcher *packetDispatcher_;

    bool trackingEnabled_{false};
    bool loggingEnabled_;
    bool replayMode_;
    std::atomic<bool> updateHandler_;

#if USE_TRACKING
    boost::circular_buffer<double> altitudeBuffer_{25};
    chrono::system_clock::time_point lastTrackingAngleUpdate_;
    serial::Serial serialPort_{};
#endif

    std::unique_ptr<IReceiver> telemetryHandler900MHz_;
    std::unique_ptr<IReceiver> telemetryHandler433MHz_;
    std::unique_ptr<IReceiver> compositeReceiver_;
    std::unique_ptr<IReceiver> newHandler_;
    std::chrono::system_clock::time_point lastIteration_;
    std::chrono::system_clock::time_point timeOfLastLinkCheck_;
    std::chrono::system_clock::time_point timeOfLastReceivedTelemetry_;
    long long int millisBetweenLastTwoPackets_;


    void moveTrackingSystem(double currentAltitude);
};


#endif // WORKER_H
