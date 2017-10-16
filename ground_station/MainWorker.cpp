#include <QThread>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include <c++/cassert>
#include "MainWorker.h"
#include "Utilities/GraphUtils.h"

using namespace std;

Worker::Worker() :
        enabled{true},
        telemetryHandler{new TelemetrySimulator()},
        telemetryLogger{"telemetry_data"},
        lastDisplayableReading{-1,
                               {0, false},
                               {0, false},
                               {0, false},
                               {0, false},
                               {0, false},
                               {0, 0, 0, false}},
        lastUIupdate{chrono::system_clock::now()} {
    
}

Worker::~Worker() {
    std::cout << "Destroying worker thread" << std::endl;
}

void Worker::run() {

    while (!QThread::currentThread()->isInterruptionRequested()) {
        //TODO: adapt sleep time so as to have proper framerate
        QThread::msleep(UIConstants::REFRESH_RATE);

        vector<RocketEvent> rocketEvents = telemetryHandler->getEvents();
        vector<TelemetryReading> data = telemetryHandler->getData();

        //TODO: Maybe change this and check if data is not empty
        assert(data.size() > 0);

        displayMostRecentTelemetry(data[data.size() - 1]);
        logData(data);

        QVector<QCPGraphData> speedDataBuffer = extractGraphData(data, speedFromReading);
        QVector<QCPGraphData> accelDataBuffer = extractGraphData(data, accelerationFromReading);

        emit newEventsReady(rocketEvents);
        emit graphDataReady(speedDataBuffer, GraphFeature::FEATURE1);
        emit graphDataReady(accelDataBuffer, GraphFeature::FEATURE2);
    }

    std::cout << "The worker has finished" << std::endl;
    telemetryLogger.close();
}

void Worker::displayMostRecentTelemetry(TelemetryReading tr) {
    chrono::system_clock::time_point currentTimePoint = chrono::system_clock::now();

    long long elapsedMillis = chrono::duration_cast<chrono::milliseconds>(currentTimePoint - lastUIupdate).count();

    if (elapsedMillis > UIConstants::NUMERICAL_VALUES_REFRESH_RATE) {
        lastUIupdate = currentTimePoint;
        emit telemetryReady(tr);
    }
}

void Worker::logData(vector<TelemetryReading> &data) {

    vector<reference_wrapper<ILoggable>> wrappedData(begin(data), end(data));
    telemetryLogger.registerData(wrappedData);

}

QVector<QCPGraphData>
Worker::extractGraphData(vector<TelemetryReading> &data, QCPGraphData (*extractionFct)(TelemetryReading)) {
    QVector<QCPGraphData> v;
    long lastTimestampSeen = 0;

    for (TelemetryReading reading : data) {
        if (lastTimestampSeen != reading.timestamp) {
            v.append(extractionFct(reading));
            lastTimestampSeen = reading.timestamp;
        }
    }

    return v;
}

