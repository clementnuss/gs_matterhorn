#include <QObject>
#include <iostream>
#include <DataHandlers/TelemetrySimulator.h>
#include <DataHandlers/Receiver/RadioReceiver.h>
#include <DataHandlers/TelemetryReplay.h>
#include <DataHandlers/StateEstimator.h>
#include "Application.h"

Application::Application(int &argc, char **argv) : qApplication_{argc, argv}, gsMainWindow_{}, workerThread_{} {
}

void Application::simpleTest() {
    std::this_thread::sleep_for(chrono::milliseconds(3000));
    std::string path{R"(C:\Users\clement\Documents\gs_matterhorn\data\Greg)"};
    cout << "Doing some TelemHandler tests" << endl;
//    worker_->defineReplayMode(path);
    std::this_thread::sleep_for(chrono::milliseconds(10000));

    worker_->defineReplayMode("helloThere");

}

void Application::run() {
    worker_ = new Worker(&gsMainWindow_);
    worker_->moveToThread(&workerThread_);

    connectSlotsAndSignals();

    // Initialize UI status fields
    worker_->emitAllStatuses();

}

int Application::exec() {
    workerThread_.start();

    gsMainWindow_.show();

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
                     &gsMainWindow_,
                     &GSMainwindow::updateTelemetry);

    QObject::connect(worker_,
                     &Worker::points3DReady,
                     &gsMainWindow_,
                     &GSMainwindow::register3DPoints);

    QObject::connect(worker_,
                     &Worker::loggingStatusReady,
                     &gsMainWindow_,
                     &GSMainwindow::updateLoggingStatus);

    QObject::connect(worker_,
                     &Worker::linkStatusReady,
                     &gsMainWindow_,
                     &GSMainwindow::updateLinkStatus);

    QObject::connect(worker_,
                     &Worker::newEventsReady,
                     &gsMainWindow_,
                     &GSMainwindow::updateEvents);

    QObject::connect(worker_,
                     &Worker::graphDataReady,
                     &gsMainWindow_,
                     &GSMainwindow::updateGraphData);

    QObject::connect(&workerThread_,
                     &QThread::started,
                     worker_,
                     &Worker::run);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::toggleLogging,
                     worker_,
                     &Worker::updateLoggingStatus);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::changePlaybackSpeed,
                     worker_,
                     &Worker::updatePlaybackSpeed);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::resetTelemetryReplayPlayback,
                     worker_,
                     &Worker::resetPlayback);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::reverseTelemetryReplayPlayback,
                     worker_,
                     &Worker::reversePlayback);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::defineReplayMode,
                     worker_,
                     &Worker::defineReplayMode);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::defineRealtimeMode,
                     worker_,
                     &Worker::defineRealtimeMode);

    QObject::connect(worker_,
                     &Worker::resetUIState,
                     &gsMainWindow_,
                     &GSMainwindow::resetUIState);}


Application::~Application() {
    workerThread_.requestInterruption();
    workerThread_.quit();
    workerThread_.wait();
}