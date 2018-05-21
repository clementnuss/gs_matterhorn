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
    qRegisterMetaType<RSSIResponse>("RSSIResponse");

    // Call worker destructor, cleaning up all resources
    QObject::connect(&workerThread_, &QThread::finished, worker_, &Worker::deleteLater);

    //TODO: change those preprocessor commands for global variables
#if USE_3D_MODULE
    QObject::connect(worker_,
                     &Worker::flightPositionChanged,
                     gsMainWindow_.get3DModule(),
                     &RootEntity::updateFlightPosition);

    QObject::connect(worker_,
                     &Worker::payloadPositionChanged,
                     gsMainWindow_.get3DModule(),
                     &RootEntity::updatePayloadPosition);
#endif

    QObject::connect(&workerThread_,
                     &QThread::started,
                     worker_,
                     &Worker::run);

    QObject::connect(worker_,
                     QOverload<SensorsPacket>::of(&Worker::dataChanged),
                     &gsMainWindow_,
                     QOverload<SensorsPacket>::of(&GSMainwindow::updateData));

    QObject::connect(worker_,
                     QOverload<EventPacket>::of(&Worker::dataChanged),
                     &gsMainWindow_,
                     QOverload<EventPacket>::of(&GSMainwindow::updateData));

    QObject::connect(worker_,
                     QOverload<GPSPacket>::of(&Worker::dataChanged),
                     &gsMainWindow_,
                     QOverload<GPSPacket>::of(&GSMainwindow::updateData));

    QObject::connect(worker_,
                     QOverload<RSSIResponse>::of(&Worker::dataChanged),
                     &gsMainWindow_,
                     QOverload<RSSIResponse>::of(&GSMainwindow::updateData));

    QObject::connect(worker_,
                     &Worker::ppsChanged,
                     &gsMainWindow_,
                     &GSMainwindow::updatePPS);

    QObject::connect(worker_,
                     &Worker::loggingStatusChanged,
                     &gsMainWindow_,
                     &GSMainwindow::updateLoggingStatus);

    QObject::connect(worker_,
                     &Worker::linkStatusChanged,
                     &gsMainWindow_,
                     &GSMainwindow::updateLinkStatus);

    QObject::connect(worker_,
                     &Worker::graphDataChanged,
                     &gsMainWindow_,
                     &GSMainwindow::updateGraphData);


    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::toggleLogging,
                     worker_,
                     &Worker::updateLoggingStatus);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::toggleTracking,
                     worker_,
                     &Worker::toggleTracking);

    QObject::connect(&gsMainWindow_,
                     &GSMainwindow::playbackSpeedChanged,
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

}


Application::~Application() {
    workerThread_.requestInterruption();
    workerThread_.quit();
    workerThread_.wait();
}