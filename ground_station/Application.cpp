#include <QObject>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include <DataHandlers/Receiver/RadioReceiver.h>
#include <DataHandlers/TelemetryReplay.h>
#include <DataHandlers/StateEstimator.h>
#include "Application.h"

Application::Application(int &argc, char **argv) : qApplication_{argc, argv}, mainWidget_{nullptr}, workerThread_{} {
}

void Application::simpleTest() {
    std::this_thread::sleep_for(chrono::milliseconds(3000));
    std::string path{R"(C:\Users\clement\Documents\gs_matterhorn\data\Greg)"};
    cout << "Doing some TelemHandler tests" << endl;
    worker_->defineCurrentRunningMode(SoftwareMode::REPLAY, path);
    std::this_thread::sleep_for(chrono::milliseconds(10000));

    worker_->defineCurrentRunningMode(SoftwareMode::REAL_TIME, "helloThere");

}

void Application::run() {
    worker_ = new Worker(&mainWidget_);
    worker_->moveToThread(&workerThread_);

    connectSlotsAndSignals();

    // Initialize UI status fields
    worker_->emitAllStatuses();
}

int Application::exec() {
    workerThread_.start();

    mainWidget_.show();

    qApplication_.exec();
}

void Application::connectSlotsAndSignals() {
    qRegisterMetaType<TelemetryReading>("TelemetryReading");
    qRegisterMetaType<QVector<QCPGraphData>>("QVector<QCPGraphData>&");
    qRegisterMetaType<vector<RocketEvent>>("vector<RocketEvent>&");
    qRegisterMetaType<GraphFeature>("GraphFeature");
    qRegisterMetaType<HandlerStatus>("HandlerStatus");


    QObject::connect(worker_,
                     &Worker::telemetryReady,
                     &mainWidget_,
                     &GSWidget::updateTelemetry);

    QObject::connect(worker_,
                     &Worker::points3DReady,
                     &mainWidget_,
                     &GSWidget::register3DPoints);

    QObject::connect(worker_,
                     &Worker::loggingStatusReady,
                     &mainWidget_,
                     &GSWidget::updateLoggingStatus);

    QObject::connect(worker_,
                     &Worker::linkStatusReady,
                     &mainWidget_,
                     &GSWidget::updateLinkStatus);

    QObject::connect(worker_,
                     &Worker::newEventsReady,
                     &mainWidget_,
                     &GSWidget::updateEvents);

    QObject::connect(worker_,
                     &Worker::graphDataReady,
                     &mainWidget_,
                     &GSWidget::updateGraphData);

    QObject::connect(&workerThread_,
                     &QThread::started,
                     worker_,
                     &Worker::run);

    QObject::connect(&mainWidget_,
                     &GSWidget::toggleLogging,
                     worker_,
                     &Worker::updateLoggingStatus);

    QObject::connect(&mainWidget_,
                     &GSWidget::changePlaybackSpeed,
                     worker_,
                     &Worker::updatePlaybackSpeed);

    QObject::connect(&mainWidget_,
                     &GSWidget::resetTelemetryReplayPlayback,
                     worker_,
                     &Worker::resetPlayback);

    QObject::connect(&mainWidget_,
                     &GSWidget::reverseTelemetryReplayPlayback,
                     worker_,
                     &Worker::reversePlayback);

}


Application::~Application() {
    workerThread_.requestInterruption();
    workerThread_.quit();
    workerThread_.wait();
}