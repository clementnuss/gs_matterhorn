#include <QThread>
#include <iostream>
#include "MainWorker.h"

Worker::Worker() : enabled{true} {

}

Worker::~Worker(){
    std::cout << "Destroying worker thread" << std::endl;
}

void Worker::run(){

    while(!QThread::currentThread()->isInterruptionRequested()){
        QThread::msleep(100);
        emit dummySignal();
    }

    std::cout << "The worker has finished" << std::endl;
}
