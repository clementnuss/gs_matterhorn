#include <QThread>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include "MainWorker.h"
#include "ProgramConstants.h"
#include "Utilities/GraphUtils.h"

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

        QVector<QCPGraphData> speedDataBuffer = extractGraphData(data, speedFromReading);
        QVector<QCPGraphData> accelDataBuffer = extractGraphData(data, accelerationFromReading);

        emit graphDataReady(speedDataBuffer, GraphFeature::FEATURE1);
        emit graphDataReady(accelDataBuffer, GraphFeature::FEATURE2);

    }

    std::cout << "The worker has finished" << std::endl;
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

