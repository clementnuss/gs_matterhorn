#include <QThread>
#include <iostream>
#include <DataHandlers/Simulator/TelemetrySimulator.h>
#include <DataHandlers/Receiver/RadioReceiver.h>
#include <DataHandlers/Replay/TelemetryReplay.h>
#include <DataHandlers/StateEstimator.h>
#include "MainWorker.h"
#include "Utilities/GraphUtils.h"
#include "Utilities/TimeUtils.h"
#include "UIWidget.h"

using namespace std;

/**
 *
 * @param telemetryHandler
 */
Worker::Worker(GSMainwindow *gsMainwindow) :
        loggingEnabled_{false},
        telemetryLogger{LogConstants::WORKER_TELEMETRY_LOG_PATH},
        eventLogger{LogConstants::WORKER_EVENTS_LOG_PATH},
        lastUIupdate{chrono::system_clock::now()},
        lastIteration{chrono::system_clock::now()},
        timeOfLastLinkCheck{chrono::system_clock::now()},
        timeOfLastReceivedTelemetry{chrono::system_clock::now()},
        millisBetweenLastTwoPackets{0},
        replayMode_{false},
        updateHandler_{false},
        telemetryHandler_{} {

    gsMainwindow->setRealTimeMode();
    TelemetryHandler *handler;
    try {
        handler = new RadioReceiver("");
        handler->startup();
    } catch (std::runtime_error &e) {
        std::cerr << "Error when starting the initial radio receiver handler:\n" << e.what();
    }

    telemetryHandler_ = unique_ptr<TelemetryHandler>{handler};

#ifdef USE_TRACKING
    vector<serial::PortInfo> devices_found = serial::list_ports();
    if (!devices_found.empty()) {
        auto iter = devices_found.begin();
        while (iter != devices_found.end()) {
            serial::PortInfo device = *iter++;
            if (device.hardware_id == "USB\\VID_2A03&PID_0042&REV_0001" /*Arduino MEGA serial hardware id*/) {
                serialPort_.setPort(device.port);
                cout << "Arduino serial port found.\n";
            }
        }
    } else {
        std::cerr << "No Serial port available, won't perform tracking!" << std::endl;
    }
    serialPort_.setBaudrate(CommunicationsConstants::TELEMETRY_BAUD_RATE);
    serialPort_.setFlowcontrol(serial::flowcontrol_none);
    serialPort_.setStopbits(serial::stopbits_one);
    serialPort_.setTimeout(1, 5, 0, 5, 0);

    try {
        serialPort_.open();
    } catch (std::exception &e) {
        cout << "Error while opening Arduino serial port: \n" << e.what();
    }

    lastTrackingAngleUpdate = chrono::system_clock::now();
#endif

}

Worker::~Worker() {
    std::cout << "Destroying worker thread" << std::endl;
}

/**
 * Emits all statuses boolean. This should be used once the UI has loaded to initialise
 * the different status color markers.
 */
void Worker::emitAllStatuses() {
    emit loggingStatusReady(loggingEnabled_);
}

/**
 * Entry point of the executing thread
 */
void Worker::run() {

    while (!(QThread::currentThread()->isInterruptionRequested())) {
        if (updateHandler_.load()) {
            loggingEnabled_ = false;
            replayMode_ = false;
            millisBetweenLastTwoPackets = 0;
            lastUIupdate = chrono::system_clock::now();
            lastIteration = chrono::system_clock::now();
            timeOfLastLinkCheck = chrono::system_clock::now();
            timeOfLastReceivedTelemetry = chrono::system_clock::now();


            telemetryHandler_.swap(newHandler_);
            newHandler_ = nullptr;
            if (telemetryHandler_->isReplayHandler()) {
                replayMode_ = true;
            }
            emit resetUIState();

            updateHandler_.store(false);
        } else {
            mainRoutine();
        }

    }

    std::cout << "The worker has finished" << std::endl;
    telemetryLogger.close();
    eventLogger.close();
}

/**
 * Work loop
 */
void Worker::mainRoutine() {
    //TODO: adapt sleep time so as to have proper framerate
    auto elapsed = msecsBetween(lastIteration, chrono::system_clock::now());
    if (elapsed < UIConstants::REFRESH_RATE) {
        QThread::msleep(UIConstants::REFRESH_RATE - static_cast<unsigned long>(elapsed));
    }
    lastIteration = chrono::system_clock::now();
    checkLinkStatuses();

    vector<RocketEvent> rocketEvents = telemetryHandler_->pollEvents();
    vector<TelemetryReading> telemReadings = telemetryHandler_->pollData();
    vector<XYZReading> geoData = telemetryHandler_->pollLocations();

    chrono::system_clock::time_point now = chrono::system_clock::now();

    if (loggingEnabled_) {
        telemetryLogger.registerData(
                std::vector<std::reference_wrapper<ILoggable>>(begin(telemReadings), end(telemReadings)));
        eventLogger.registerData(
                std::vector<std::reference_wrapper<ILoggable>>(begin(rocketEvents), end(rocketEvents)));
    }

    if (!telemReadings.empty()) {
#ifdef USE_TRACKING
        moveTrackingSystem((*--telemReadings.end()).altitude_);
#endif

        millisBetweenLastTwoPackets = msecsBetween(timeOfLastReceivedTelemetry, now);
        timeOfLastReceivedTelemetry = now;
        displayMostRecentTelemetry(*--telemReadings.end());

        QVector<QCPGraphData> altitudeDataBuffer = extractGraphData(telemReadings, altitudeFromReading);
        QVector<QCPGraphData> accelDataBuffer = extractGraphData(telemReadings, accelerationFromReading);
        emit graphDataReady(altitudeDataBuffer, GraphFeature::FEATURE1);
        emit graphDataReady(accelDataBuffer, GraphFeature::FEATURE2);
    } else {
        if (replayMode_) {
            auto *telemReplay = dynamic_cast<ITelemetryReplayHandler *>(telemetryHandler_.get());
            if (!telemReplay->endOfPlayback()) {
                QVector<QCPGraphData> empty;
                emit graphDataReady(empty, GraphFeature::Count);
            }
        } else {
            QVector<QCPGraphData> empty;
            emit graphDataReady(empty, GraphFeature::Count);
        }
    }

    if (!geoData.empty()) {
        QVector<QVector3D> v{};
        for (auto geoPoint : geoData) {
            v.append(QVector3D{static_cast<float>(geoPoint.x_),
                               static_cast<float>(geoPoint.y_),
                               static_cast<float>(geoPoint.z_)});
        }
        //emit points3DReady(v);
    }

    if (!rocketEvents.empty()) {
        emit newEventsReady(rocketEvents);
    }

    QCoreApplication::sendPostedEvents(this);
    QCoreApplication::processEvents();
}

/**
 * Emits a boolean to the UI indicating the current status of the logger.
 */
void Worker::updateLoggingStatus() {

    loggingEnabled_ = !loggingEnabled_;
    emit loggingStatusReady(loggingEnabled_);
    cout << "Logging is now " << (loggingEnabled_ ? "enabled" : "disabled") << endl;
}


void Worker::toggleTracking() {
    trackingEnabled_ = !trackingEnabled_;

    SensorConstants::launchAltitude = static_cast<float>(lastAltitude);
    SensorConstants::trackingAltitude = static_cast<float>(lastAltitude + 1);
}

/**
 * Determine the status of the communication based on the quantity of data received during the past second.
 */
void Worker::checkLinkStatuses() {
    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(timeOfLastLinkCheck, now);

    if (elapsedMillis > CommunicationsConstants::MSECS_BETWEEN_LINK_CHECKS) {

        timeOfLastLinkCheck = now;

        elapsedMillis = msecsBetween(timeOfLastReceivedTelemetry, now);

        HandlerStatus status;

        if (elapsedMillis > CommunicationsConstants::MSECS_LOSSY_RATE
            || millisBetweenLastTwoPackets > CommunicationsConstants::MSECS_LOSSY_RATE) {
            status = HandlerStatus::DOWN;
        } else if (CommunicationsConstants::MSECS_NOMINAL_RATE < millisBetweenLastTwoPackets
                   && millisBetweenLastTwoPackets <= CommunicationsConstants::MSECS_LOSSY_RATE) {
            status = HandlerStatus::LOSSY;
        } else {
            status = HandlerStatus::NOMINAL;
        }

        emit linkStatusReady(status);
    }
}

/**
 * Emits to the UI the latest telemetry data. If the interval between two calls to this function is lower
 * than the program constant regulating the UI telemetry refresh rate then the function has no effect.
 *
 * @param tr The telemetry struct to be displayed.
 */
void Worker::displayMostRecentTelemetry(TelemetryReading tr) {

    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(lastUIupdate, now);

    if (elapsedMillis > UIConstants::NUMERICAL_VALUES_REFRESH_RATE) {
        lastUIupdate = now;
        emit telemetryReady(tr);
    }
}


/**
 *
 *
 * @param data A reference to a vector of telemetry structs.
 * @param extractionFct A helper function to convert the strucs to plottable objects (QCPGraphData).
 * @return A QVector of QCPGraphData.
 */
QVector<QCPGraphData>
Worker::extractGraphData(vector<TelemetryReading> &data, QCPGraphData (*extractionFct)(TelemetryReading)) {
    QVector<QCPGraphData> v;
    long long int lastTimestampSeen = 0;

    for (TelemetryReading reading : data) {
        if (abs(lastTimestampSeen - reading.timestamp_) > UIConstants::GRAPH_DATA_INTERVAL_USECS) {
            v.append(extractionFct(reading));
            lastTimestampSeen = reading.timestamp_;
        }
    }

    return v;
}

void Worker::updatePlaybackSpeed(double newSpeed) {
    assert(replayMode_);
    auto *telemReplay = dynamic_cast<ITelemetryReplayHandler *>(telemetryHandler_.get());
    telemReplay->updatePlaybackSpeed(newSpeed);
}

void Worker::resetPlayback() {
    assert(replayMode_);
    auto *telemReplay = dynamic_cast<ITelemetryReplayHandler *>(telemetryHandler_.get());
    telemReplay->resetPlayback();
}

void Worker::reversePlayback(bool reversed) {
    assert(replayMode_);
    auto *telemReplay = dynamic_cast<ITelemetryReplayHandler *>(telemetryHandler_.get());
    telemReplay->setPlaybackReversed(reversed);
}

//TODO: determine wether a non working handler should be used or not


void Worker::defineReplayMode(const QString &parameters) {
    TelemetryHandler *handler = nullptr;
    try {
        handler = new TelemetryReplay(parameters.toStdString());
        handler->startup();
    } catch (std::runtime_error &e) {
        std::cerr << "Error when starting replay handler:\n" << e.what();
    }
    newHandler_ = unique_ptr<TelemetryHandler>{handler};
    updateHandler_.store(true);
}


void Worker::defineRealtimeMode(const QString &parameters) {
    TelemetryHandler *handler = nullptr;
    try {
        handler = new RadioReceiver(parameters.toStdString());
        handler->startup();
    } catch (std::runtime_error &e) {
        std::cerr << "Error when starting RadioReceiver handler:\n" << e.what();
    }
    newHandler_ = unique_ptr<TelemetryHandler>{handler};
    updateHandler_.store(true);
}

void Worker::moveTrackingSystem(double currentAltitude) {
    lastAltitude = currentAltitude;
#if USE_TRACKING
    /**
     * We use the law of sines to get the correct angle. The tracking altitude corresponds to the ground station's
     * relative elevation with regards to the launch site altitude.
     * The distanceToLaunchSite variable is the horizontal distance between the launch site and the GS.
     */

    double beta = atan(SensorConstants::distanceToLaunchSite / abs(currentAltitude - SensorConstants::launchAltitude));
    double hypothenuseToLaunchSite = sqrt(
            pow(SensorConstants::distanceToLaunchSite, 2) +
            pow(SensorConstants::trackingAltitude - currentAltitude, 2));
    double gamma = asin(abs(currentAltitude - SensorConstants::launchAltitude) * sin(beta) / hypothenuseToLaunchSite);

    gamma *= 180 / M_PI;
    gamma = 123 - gamma;
    angleBuffer_.push_back((int) gamma);


    float movAverage = 0;
    for (int i: angleBuffer_) {
        movAverage += i;
    }
    movAverage /= angleBuffer_.size();

    if (msecsBetween(lastTrackingAngleUpdate, chrono::system_clock::now()) > 100) {
        lastTrackingAngleUpdate = chrono::system_clock::now();
        std::string toSend = std::to_string(static_cast<int>(movAverage)) + "\r\n";
        cout << "angle for tracking: " << toSend;
        if (trackingEnabled_) {
            serialPort_.write(toSend);
            uint8_t carriageReturn = 0x0d;
            serialPort_.write(&carriageReturn, 1);
        }
    }

#endif
}
