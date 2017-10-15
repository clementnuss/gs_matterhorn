#include <QThread>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include "MainWorker.h"
#include "ProgramConstants.h"

using namespace std;

Worker::Worker() : enabled{true}, telemetryHandler{new TelemetrySimulator()} {

}

Worker::~Worker(){
    std::cout << "Destroying worker thread" << std::endl;
}

void Worker::run(){

    while(!QThread::currentThread()->isInterruptionRequested()){
        //TODO: adapt sleep time so as to have proper 30 fps
        QThread::msleep(UIConstants::REFRESH_RATE);

        vector<TelemetryReading> data = telemetryHandler->getData();
        appendAccelData(data);
        appendSpeedData(data);
        emit graphDataReady(speedDataBuffer, GraphFeature::FEATURE1);
        emit graphDataReady(accelDataBuffer, GraphFeature::FEATURE2);

    }

    std::cout << "The worker has finished" << std::endl;
}

void Worker::appendSpeedData(vector<TelemetryReading> &data) {

    //TODO: plot only one value per msec ?
    for (TelemetryReading reading : data) {
        speedDataBuffer.append(QCPGraphData(reading.timestamp, reading.speed.value));
    }

    // Remove older elements so the graph does not have to store them
    if (speedDataBuffer.size() > DataConstants::MAX_DATA_VECTOR_SIZE) {
        speedDataBuffer.remove(0, speedDataBuffer.size() - DataConstants::MAX_DATA_VECTOR_SIZE);
    }
}

void Worker::appendAccelData(vector<TelemetryReading> &data) {

    for (TelemetryReading reading : data) {
        accelDataBuffer.append(QCPGraphData(reading.timestamp, reading.acceleration.value));
    }

    // Remove older elements so the graph does not have to store them
    if (accelDataBuffer.size() > DataConstants::MAX_DATA_VECTOR_SIZE) {
        accelDataBuffer.remove(0, accelDataBuffer.size() - DataConstants::MAX_DATA_VECTOR_SIZE);
    }
}

