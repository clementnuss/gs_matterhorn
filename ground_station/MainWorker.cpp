#include <QThread>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include <DataHandlers/Receiver/RadioReceiver.h>
#include "MainWorker.h"
#include "Utilities/GraphUtils.h"
#include "Utilities/TimeUtils.h"

using namespace std;

Worker::Worker(std::string comPort) :
        loggingEnabled{false},
        telemetryHandler{new RadioReceiver{"COM7"}},
        telemetryLogger{LogConstants::TELEMETRY_PATH},
        eventLogger{LogConstants::EVENTS_PATH},
        lastDisplayableReading{0,
                               0,
                               {0, 0, 0},
                               {0, 0, 0},
                               {0, 0, 0},
                               0,
                               0},
        lastUIupdate{chrono::system_clock::now()},
        timeOfLastLinkCheck{chrono::system_clock::now()},
        timeOfLastReceivedTelemetry{chrono::system_clock::now()},
        millisBetweenLastTwoPackets{0} {
    //TODO: catch error
    telemetryHandler->startup();

}

Worker::~Worker() {
    std::cout << "Destroying worker thread" << std::endl;
}

void Worker::emitAllStatuses() {
    emit loggingStatusReady(loggingEnabled);
}

void Worker::run() {

    while (!QThread::currentThread()->isInterruptionRequested()) {
        mainRoutine();
    }

    std::cout << "The worker has finished" << std::endl;
    telemetryLogger.close();
    eventLogger.close();
}

void Worker::mainRoutine() {
    //TODO: adapt sleep time so as to have proper framerate
    QThread::msleep(UIConstants::REFRESH_RATE);

    checkLinkStatuses();

    vector<RocketEvent> rocketEvents = telemetryHandler->pollEvents();
    vector<TelemetryReading> data = telemetryHandler->pollData();

    chrono::system_clock::time_point now = chrono::system_clock::now();

    if (loggingEnabled) {
        telemetryLogger.registerData(vector<reference_wrapper<ILoggable>>(begin(data), end(data)));
        eventLogger.registerData(vector<reference_wrapper<ILoggable>>(begin(rocketEvents), end(rocketEvents)));
    }

    if (!data.empty()) {
        millisBetweenLastTwoPackets = msecsBetween(timeOfLastReceivedTelemetry, now);
        timeOfLastReceivedTelemetry = now;
        displayMostRecentTelemetry(data[data.size() - 1]);

        QVector<QCPGraphData> altitudeDataBuffer = extractGraphData(data, altitudeFromReading);
        QVector<QCPGraphData> accelDataBuffer = extractGraphData(data, accelerationFromReading);

        emit graphDataReady(altitudeDataBuffer, GraphFeature::FEATURE1);
        emit graphDataReady(accelDataBuffer, GraphFeature::FEATURE2);
    }

    if (!rocketEvents.empty()) {
        emit newEventsReady(rocketEvents);
    }

    QCoreApplication::sendPostedEvents(this);
    QCoreApplication::processEvents();
}

void Worker::updateLoggingStatus() {
    loggingEnabled = !loggingEnabled;
    emit loggingStatusReady(loggingEnabled);
    cout << "Logging is now " << (loggingEnabled ? "enabled" : "disabled") << endl;
}

void Worker::checkLinkStatuses() {
    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(timeOfLastLinkCheck, now);

    if (elapsedMillis > CommunicationsConstants::MSECS_BETWEEN_LINK_CHECKS) {

        timeOfLastLinkCheck = now;

        elapsedMillis = msecsBetween(timeOfLastReceivedTelemetry, now);

        HandlerStatus status;

        if (elapsedMillis > CommunicationsConstants::MSECS_LOSSY_RATE
            || millisBetweenLastTwoPackets > CommunicationsConstants::MSECS_LOSSY_RATE) {
            status = HandlerStatus::DOWN;
        } else if (CommunicationsConstants::MSECS_NOMINAL_RATE < millisBetweenLastTwoPackets
                   && millisBetweenLastTwoPackets <= CommunicationsConstants::MSECS_LOSSY_RATE) {
            status = HandlerStatus::LOSSY;
        } else {
            status = HandlerStatus::NOMINAL;
        }

        emit linkStatusReady(status);
    }
}

//TODO: harmonise with graph updates
void Worker::displayMostRecentTelemetry(TelemetryReading tr) {

    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(lastUIupdate, now);

    if (elapsedMillis > UIConstants::NUMERICAL_VALUES_REFRESH_RATE) {
        lastUIupdate = now;
        emit telemetryReady(tr);
    }
}


QVector<QCPGraphData>
Worker::extractGraphData(vector<TelemetryReading> &data, QCPGraphData (*extractionFct)(TelemetryReading)) {
    QVector<QCPGraphData> v;
    long lastTimestampSeen = 0;

    for (TelemetryReading reading : data) {
        if (abs(lastTimestampSeen - reading.timestamp_) > UIConstants::GRAPH_DATA_INTERVAL_MSECS) {
            v.append(extractionFct(reading));
            lastTimestampSeen = reading.timestamp_;
        }
    }

    return v;
}


