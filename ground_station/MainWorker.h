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

class PacketDispatcher_Nested;

class Worker : public QObject {
Q_OBJECT

public:

    class PacketDispatcher {
    public:
        PacketDispatcher(const Worker *const container);

        virtual void dispatch(SensorsPacket *) const;

        virtual void dispatch(GPSPacket *) const;

    private:
        const Worker *const container_;
    };


    explicit Worker(GSMainwindow
                    *);

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

    void sensorsDataReady(SensorsPacket *) const;

    void eventDataReady(EventPacket *) const;

    void gpsDataReady(GPSPacket *) const;

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

    void displaySensorData(SensorsPacket *);

    void displayEventData(EventPacket *);

    void displayGPSData(GPSPacket *);

    const PacketDispatcher_Nested *packetDispatcher_;

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
    std::unique_ptr<IReceiver> newHandler_;
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
    std::chrono::system_clock::time_point lastNumericalValuesUpdateRocket_;
    std::chrono::system_clock::time_point lastNumericalValuesUpdatePayload_;
    std::chrono::system_clock::time_point lastIteration_;
    std::chrono::system_clock::time_point timeOfLastLinkCheck_;
    std::chrono::system_clock::time_point timeOfLastReceivedTelemetry_;
    long long int millisBetweenLastTwoPackets_;

    QVector<QCPGraphData> extractGraphData(std::vector<SensorsPacket> &, QCPGraphData (*)(SensorsPacket));

    void moveTrackingSystem(double currentAltitude);
};

// Needed because of the C++ limitation in forward-declaring nested classes
class PacketDispatcher_Nested : public Worker::PacketDispatcher {
    // Inherits base constructor (PacketDispatcher)
    using Worker::PacketDispatcher::PacketDispatcher;
};

#endif // WORKER_H
