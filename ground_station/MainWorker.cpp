#include <QThread>
#include <iostream>
#include "MainWorker.h"

using namespace std;

Worker::Worker() : enabled{true} {

}

Worker::~Worker(){
    std::cout << "Destroying worker thread" << std::endl;
}

void Worker::run(){

    while(!QThread::currentThread()->isInterruptionRequested()){
        QThread::msleep(1000);


        vector<TelemetryReading> telemetryData = telemetryHandler.getData();
        emit graphDataReady(graphSpeedData(telemetryData), GraphFeature::FEATURE1);

    }

    std::cout << "The worker has finished" << std::endl;
}

QVector<QCPGraphData> Worker::graphSpeedData(vector<TelemetryReading> &data) {
    return QVector<QCPGraphData>();
}

