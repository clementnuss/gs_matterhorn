#include <QThread>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include "MainWorker.h"
#include "Utilities/GraphUtils.h"

using namespace std;

Worker::Worker() :
        enabled{true},
        telemetryHandler{new TelemetrySimulator()},
        telemetryLogger{LogConstants::TELEMETRY_PATH},
        eventLogger{LogConstants::EVENTS_PATH},
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
        mainRoutine();
    }

    std::cout << "The worker has finished" << std::endl;
    telemetryLogger.close();
    eventLogger.close();
}

void Worker::mainRoutine() {
    //TODO: adapt sleep time so as to have proper framerate
    QThread::msleep(UIConstants::REFRESH_RATE);

    vector<RocketEvent> rocketEvents = telemetryHandler->getEvents();
    vector<TelemetryReading> data = telemetryHandler->getData();

    telemetryLogger.registerData(vector<reference_wrapper<ILoggable>>(begin(data), end(data)));
    eventLogger.registerData(vector<reference_wrapper<ILoggable>>(begin(rocketEvents), end(rocketEvents)));

    displayMostRecentTelemetry(data[data.size() - 1]);

    QVector<QCPGraphData> speedDataBuffer = extractGraphData(data, speedFromReading);
    QVector<QCPGraphData> accelDataBuffer = extractGraphData(data, accelerationFromReading);

    emit newEventsReady(rocketEvents);
    emit graphDataReady(speedDataBuffer, GraphFeature::FEATURE1);
    emit graphDataReady(accelDataBuffer, GraphFeature::FEATURE2);
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
        if (lastTimestampSeen != reading.timestamp) {
            v.append(extractionFct(reading));
            lastTimestampSeen = reading.timestamp;
        }
    }

    return v;
}

