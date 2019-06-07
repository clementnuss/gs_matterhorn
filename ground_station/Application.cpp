#include "Application.h"
#include "MainWorker.h"

void
Application::run() {

    wrkr_.run();

}

int
Application::exec() {

	return 0;
}

/*
void
Application::connectSlotsAndSignals() {

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
						 &Worker::ppsOnPrimaryRFChanged,
						 &gsMainWindow_,
						 &GSMainwindow::updatePrimaryRFPPS);

		QObject::connect(worker_,
						 &Worker::ppsOnSecondaryRFChanged,
						 &gsMainWindow_,
						 &GSMainwindow::updateSecondaryRFPPS);

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
						 
}*/

Application::~Application() {
}