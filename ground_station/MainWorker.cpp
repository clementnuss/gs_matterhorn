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
//        telemetryHandler{new RadioReceiver{comPort}},
        telemetryHandler{new TelemetrySimulator()},
        telemetryLogger{LogConstants::WORKER_TELEMETRY_LOG_PATH},
        eventLogger{LogConstants::WORKER_EVENTS_LOG_PATH},
        lastDisplayableReading{0,
                               0,
                               {0, 0, 0},
                               {0, 0, 0},
                               {0, 0, 0},
                               0,
                               0,
                               0,
                               0},
        lastUIupdate{chrono::system_clock::now()},
        lastIteration{chrono::system_clock::now()},
        timeOfLastLinkCheck{chrono::system_clock::now()},
        timeOfLastReceivedTelemetry{chrono::system_clock::now()},
        millisBetweenLastTwoPackets{0} {
    //TODO: catch error
    telemetryHandler->startup();

}

Worker::~Worker() {
    std::cout << "Destroying worker thread" << std::endl;
}

/**
 * Emits all statuses boolean. This should be used once the UI has loaded to initialise
 * the different status color markers.
 */
void Worker::emitAllStatuses() {
    emit loggingStatusReady(loggingEnabled);
}

/**
 * Entry point of the executing thread
 */
void Worker::run() {

    while (!QThread::currentThread()->isInterruptionRequested()) {
        mainRoutine();
    }

    std::cout << "The worker has finished" << std::endl;
    telemetryLogger.close();
    eventLogger.close();
}

/**
 * Work loop
 */
void Worker::mainRoutine() {
    //TODO: adapt sleep time so as to have proper framerate
    auto elapsed = msecsBetween(lastIteration, chrono::system_clock::now());
    if (elapsed < UserIfaceConstants::REFRESH_RATE) {
        QThread::msleep(UserIfaceConstants::REFRESH_RATE - static_cast<unsigned long>(elapsed));
    }
    lastIteration = chrono::system_clock::now();
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
    } else {
        QVector<QCPGraphData> empty;
        emit graphDataReady(empty, GraphFeature::Count);
    }

    if (!rocketEvents.empty()) {
        emit newEventsReady(rocketEvents);
    }

    QCoreApplication::sendPostedEvents(this);
    QCoreApplication::processEvents();
}

/**
 * Emits a boolean to the UI indicating the current status of the logger.
 */
void Worker::updateLoggingStatus() {

    loggingEnabled = !loggingEnabled;
    emit loggingStatusReady(loggingEnabled);
    cout << "Logging is now " << (loggingEnabled ? "enabled" : "disabled") << endl;
}

/**
 * Determine the status of the communication based on the quantity of data received during the past second.
 */
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

/**
 * Emits to the UI the latest telemetry data. If the interval between two calls to this function is lower
 * than the program constant regulating the UI telemetry refresh rate then the function has no effect.
 *
 * @param tr The telemetry struct to be displayed.
 */
//TODO: harmonise with graph updates
void Worker::displayMostRecentTelemetry(TelemetryReading tr) {

    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(lastUIupdate, now);

    if (elapsedMillis > UserIfaceConstants::NUMERICAL_VALUES_REFRESH_RATE) {
        lastUIupdate = now;
        emit telemetryReady(tr);
    }
}


/**
 *
 *
 * @param data A reference to a vector of telemetry structs.
 * @param extractionFct A helper function to convert the strucs to plottable objects (QCPGraphData).
 * @return A QVector of QCPGraphData.
 */
QVector<QCPGraphData>
Worker::extractGraphData(vector<TelemetryReading> &data, QCPGraphData (*extractionFct)(TelemetryReading)) {
    QVector<QCPGraphData> v;
    long long int lastTimestampSeen = 0;

    for (TelemetryReading reading : data) {
        if (abs(lastTimestampSeen - reading.timestamp_) > UserIfaceConstants::GRAPH_DATA_INTERVAL_USECS) {
            v.append(extractionFct(reading));
            lastTimestampSeen = reading.timestamp_;
        }
    }

    return v;
}


