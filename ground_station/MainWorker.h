#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <DataStructures/Datastructs.h>
#include <DataHandlers/IReceiver.h>
#include <chrono>
#include <serial/serial.h>
#include <boost/circular_buffer.hpp>
#include <DataHandlers/Receiver/CompositeReceiver.h>
#include "Loggers/FileLogger.h"
#include "Flyable.h"


class Worker {

public:

    Worker();

    ~Worker();

    void mainRoutine();

    void run();

    void transmitCommand(int);

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
    std::unique_ptr<CompositeReceiver> compositeReceiver_;
    std::unique_ptr<IReceiver> newHandler_;
    std::chrono::system_clock::time_point lastIteration_;
    std::chrono::system_clock::time_point timeOfLastLinkCheck_;
    std::chrono::system_clock::time_point timeOfLastReceivedTelemetry_;
    long long int millisBetweenLastTwoPackets_;

};


#endif // WORKER_H
