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
        //TODO: adapt sleep time so as to have proper 30 fps
        QThread::msleep(UIConstants::REFRESH_RATE);

        vector<TelemetryReading> data = telemetryHandler->getData();

        assert(data.size() > 0);

        displayMostRecentTelemetry(data[data.size() - 1]);
        logData(data);

        QVector<QCPGraphData> speedDataBuffer = extractGraphData(data, speedFromReading);
        QVector<QCPGraphData> accelDataBuffer = extractGraphData(data, accelerationFromReading);

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
    //TODO: plot only one value per msec ?
    for (TelemetryReading reading : data) {
        v.append(extractionFct(reading));
    }

    return v;
}

