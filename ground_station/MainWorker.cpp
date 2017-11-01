#include <QThread>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include <DataHandlers/Receiver/RadioReceiver.h>
#include "MainWorker.h"
#include "Utilities/GraphUtils.h"

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
        lastUIupdate{chrono::system_clock::now()} {
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

    vector<RocketEvent> rocketEvents = telemetryHandler->pollEvents();
    vector<TelemetryReading> data = telemetryHandler->pollData();

    if (loggingEnabled) {
        telemetryLogger.registerData(vector<reference_wrapper<ILoggable>>(begin(data), end(data)));
        eventLogger.registerData(vector<reference_wrapper<ILoggable>>(begin(rocketEvents), end(rocketEvents)));
    }

    if (!data.empty()) {
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

void Worker::displayMostRecentTelemetry(TelemetryReading tr) {
    chrono::system_clock::time_point currentTimePoint = chrono::system_clock::now();

    long long elapsedMillis = chrono::duration_cast<chrono::milliseconds>(currentTimePoint - lastUIupdate).count();

    if (elapsedMillis > UIConstants::NUMERICAL_VALUES_REFRESH_RATE) {
        lastUIupdate = currentTimePoint;
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

