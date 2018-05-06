#include <QObject>
#include "Application.h"

Application::Application(int &argc, char **argv) : qApplication_{argc, argv}, gsMainWindow_{}, workerThread_{} {
}

void
Application::run() {
    worker_ = new Worker(&gsMainWindow_);
    worker_->moveToThread(&workerThread_);

    connectSlotsAndSignals();

    // Initialize UI status fields
    worker_->emitAllStatuses();

}

int
Application::exec() {
    workerThread_.start();

    gsMainWindow_.show();

    qApplication_.exec();
}

void
Application::connectSlotsAndSignals() {
    qRegisterMetaType<SensorsPacket>("SensorsPacket");
    qRegisterMetaType<GPSPacket>("GPSPacket");
    qRegisterMetaType<EventPacket>("EventPacket");
    qRegisterMetaType<ControlPacket>("ControlPacket");
    qRegisterMetaType<QVector<QCPGraphData>>("QVector<QCPGraphData>&");
    qRegisterMetaType<vector<EventPacket>>("vector<RocketEvent>&");
    qRegisterMetaType<GraphFeature>("GraphFeature");
    qRegisterMetaType<HandlerStatus>("HandlerStatus");
    qRegisterMetaType<Position>("Position");
    qRegisterMetaType<FlyableType>("FlyableType");

    // Call worker destructor, cleaning up all resources
    QObject::connect(&workerThread_, &QThread::finished, worker_, &Worker::deleteLater);

    //TODO: change those preprocessor commands for global variables
#if USE_3D_MODULE
    QObject::connect(worker_,
                     &Worker::flightPositionReady,
                     gsMainWindow_.get3DModule(),
                     &RootEntity::updateFlightPosition);

    QObject::connect(worker_,
                     &Worker::payloadPositionReady,
                     gsMainWindow_.get3DModule(),
                     &RootEntity::updatePayloadPosition);
#endif

    QObject::connect(&workerThread_,
                     &QThread::started,
                     worker_,
                     &Worker::run);
/*
    QObject::connect(worker_,
                     &Worker::sensorsDataReady,
                     &gsMainWindow_,
                     &GSMainwindow::receiveSensorData);

    QObject::connect(worker_,
                     &Worker::eventDataReady,
                     &gsMainWindow_,
                     &GSMainwindow::receiveEventData);

    QObject::connect(worker_,
                     &Worker::gpsDataReady,
                     &gsMainWindow_,
                     &GSMainwindow::receiveGPSData);

    QObject::connect(worker_,
                     &Worker::loggingStatusReady,
                     &gsMainWindow_,
                     &GSMainwindow::updateLoggingStatus);

    QObject::connect(worker_,
                     &Worker::linkStatusReady,
                     &gsMainWindow_,
                     &GSMainwindow::updateLinkStatus);

    QObject::connect(worker_,
                     &Worker::graphDataReady,
                     &gsMainWindow_,
                     &GSMainwindow::receiveGraphData);



    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::toggleLogging,
                     worker_,
                     &Worker::updateLoggingStatus);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::toggleTracking,
                     worker_,
                     &Worker::toggleTracking);

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
                     &GSMainwindow::resetUIState);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::sendCommand,
                     worker_,
                     &Worker::transmitCommand);
                     */
}


Application::~Application() {
    workerThread_.requestInterruption();
    workerThread_.quit();
    workerThread_.wait();
}