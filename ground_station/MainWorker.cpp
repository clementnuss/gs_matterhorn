#include <QThread>
#include <iostream>
#include <DataHandlers/Simulator/TelemetrySimulator.h>
#include <DataHandlers/Receiver/RadioReceiver.h>
#include <DataHandlers/Replay/TelemetryReplay.h>
#include <DataHandlers/StateEstimator.h>
#include "MainWorker.h"
#include "Utilities/GraphUtils.h"
#include "Utilities/TimeUtils.h"
#include "UI/UIWidget.h"

using namespace std;

/**
 *
 * @param telemetryHandler
 */
Worker::Worker(GSMainwindow *gsMainwindow) :
        loggingEnabled_{false},
        sensorsLogger_{LogConstants::WORKER_TELEMETRY_LOG_PATH},
        eventsLogger_{LogConstants::WORKER_EVENTS_LOG_PATH},
        gpsLogger_{LogConstants::WORKER_GPS_LOG_PATH},
        lastNumericalValuesUpdate_{chrono::system_clock::now()},
        lastIteration_{chrono::system_clock::now()},
        timeOfLastLinkCheck_{chrono::system_clock::now()},
        timeOfLastReceivedTelemetry_{chrono::system_clock::now()},
        millisBetweenLastTwoPackets_{0},
        replayMode_{false},
        updateHandler_{false},
        lastComputedPosition_{},
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

#if USE_TRACKING
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

    lastTrackingAngleUpdate_ = chrono::system_clock::now();
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
            millisBetweenLastTwoPackets_ = 0;
            lastNumericalValuesUpdate_ = chrono::system_clock::now();
            lastIteration_ = chrono::system_clock::now();
            timeOfLastLinkCheck_ = chrono::system_clock::now();
            timeOfLastReceivedTelemetry_ = chrono::system_clock::now();


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
    sensorsLogger_.close();
    eventsLogger_.close();
}

/**
 * Work loop
 */
void Worker::mainRoutine() {
    //TODO: adapt sleep time so as to have proper framerate
    auto elapsed = msecsBetween(lastIteration_, chrono::system_clock::now());
    if (elapsed < UIConstants::REFRESH_RATE) {
        QThread::msleep(UIConstants::REFRESH_RATE - static_cast<unsigned long>(elapsed));
    }
    lastIteration_ = chrono::system_clock::now();
    checkLinkStatuses();

    //Sensor data needs to be polled first!
    vector<SensorsPacket> sensorsData = telemetryHandler_->pollSensorsData();
    vector<EventPacket> eventsData = telemetryHandler_->pollEventsData();
    vector<GPSPacket> gpsData = telemetryHandler_->pollGPSData();

    chrono::system_clock::time_point now = chrono::system_clock::now();

    if (loggingEnabled_) {
        sensorsLogger_.registerData(
                std::vector<std::reference_wrapper<ILoggable>>(begin(sensorsData), end(sensorsData)));
        eventsLogger_.registerData(
                std::vector<std::reference_wrapper<ILoggable>>(begin(eventsData), end(eventsData)));
        gpsLogger_.registerData(
                std::vector<std::reference_wrapper<ILoggable>>(begin(gpsData), end(gpsData)));
    }

    if (!sensorsData.empty()) {
        SensorsPacket &lastSensorValue = *--sensorsData.end();

#ifdef USE_TRACKING
        moveTrackingSystem(lastSensorValue.altitude_);
#endif
        lastComputedPosition_.altitude = lastSensorValue.altitude_;

        millisBetweenLastTwoPackets_ = msecsBetween(timeOfLastReceivedTelemetry_, now);
        timeOfLastReceivedTelemetry_ = now;
        displaySensorData(lastSensorValue);

        QVector<QCPGraphData> altitudeDataBuffer = extractGraphData(sensorsData, altitudeFromReading);
        QVector<QCPGraphData> accelDataBuffer = extractGraphData(sensorsData, accelerationFromReading);
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

    for (auto &d : eventsData) {
        displayEventData(d);
    }

    for (auto &d : gpsData) {
        displayGPSData(d);
    }

    QCoreApplication::sendPostedEvents(this);
    QCoreApplication::processEvents();
}

/**
 * Emits to the UI the latest sensors data. If the interval between two calls to this function is lower
 * than the program constant regulating the UI sensors refresh rate then the function has no effect.
 *
 * @param sp The SensorPacket to be displayed.
 */
void Worker::displaySensorData(SensorsPacket &sp) {

    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(lastNumericalValuesUpdate_, now);

    if (elapsedMillis > UIConstants::NUMERICAL_SENSORS_VALUES_REFRESH_RATE) {
        lastNumericalValuesUpdate_ = now;
        emit sensorsDataReady(sp);
    }
}

/**
 * Emits to the UI the latest event data. If the event has already been registered then the function has no effect.
 *
 * @param ep The EventPacket to be displayed.
 */
void Worker::displayEventData(EventPacket &ep) {

    if (ep.timestamp_ != lastEventTimestamp_) {
        lastEventTimestamp_ = ep.timestamp_;
        emit eventDataReady(ep);
    }
}

/**
 * Emits to the UI the latest sensors data. If the gps data has already been registered then the function has no effect.
 *
 * @param gp The GPSPacket to be displayed.
 */
void Worker::displayGPSData(GPSPacket &gp) {

    if (gp.timestamp_ != lastGPSTimestamp_) {
        lastGPSTimestamp_ = gp.timestamp_;
        if (gp.isValid()) {
            lastComputedPosition_.latLon = {gp.latitude_, gp.longitude_};
#if USE_3D_MODULE
            emit flightPositionReady(lastComputedPosition_);
#endif
        }
        emit gpsDataReady(gp);
    }
}

/**
 * Emits a boolean to the UI indicating the current status of the logger.
 */
void Worker::updateLoggingStatus() {

    if (loggingEnabled_) {
        gpsLogger_.close();
        sensorsLogger_.close();
        eventsLogger_.close();
    }

    loggingEnabled_ = !loggingEnabled_;
    emit loggingStatusReady(loggingEnabled_);
    cout << "Logging is now " << (loggingEnabled_ ? "enabled" : "disabled") << endl;
}


void Worker::toggleTracking() {
    trackingEnabled_ = !trackingEnabled_;

#if USE_TRACKING

    double movAverage = 0;
    for (double i: altitudeBuffer_) {
        movAverage += i;
    }
    movAverage /= altitudeBuffer_.size();

    SensorConstants::launchAltitude = static_cast<float>(movAverage);
    SensorConstants::trackingAltitude = static_cast<float>(movAverage + 1);
#endif
}

/**
 * Determine the status of the communication based on the quantity of data received during the past second.
 */
void Worker::checkLinkStatuses() {
    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(timeOfLastLinkCheck_, now);

    if (elapsedMillis > CommunicationsConstants::MSECS_BETWEEN_LINK_CHECKS) {

        timeOfLastLinkCheck_ = now;

        elapsedMillis = msecsBetween(timeOfLastReceivedTelemetry_, now);

        HandlerStatus status;

        if (elapsedMillis > CommunicationsConstants::MSECS_LOSSY_RATE
            || millisBetweenLastTwoPackets_ > CommunicationsConstants::MSECS_LOSSY_RATE) {
            status = HandlerStatus::DOWN;
        } else if (CommunicationsConstants::MSECS_NOMINAL_RATE < millisBetweenLastTwoPackets_
                   && millisBetweenLastTwoPackets_ <= CommunicationsConstants::MSECS_LOSSY_RATE) {
            status = HandlerStatus::LOSSY;
        } else {
            status = HandlerStatus::NOMINAL;
        }

        emit linkStatusReady(status);
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
Worker::extractGraphData(vector<SensorsPacket> &data, QCPGraphData (*extractionFct)(SensorsPacket)) {
    QVector<QCPGraphData> v;
    long long int lastTimestampSeen = 0;

    for (SensorsPacket reading : data) {
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

//TODO: determine whether a non working handler should be used or not


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
#if USE_TRACKING

    altitudeBuffer_.push_back(currentAltitude);
    double movAverage = 0;
    for (double i: altitudeBuffer_) {
        movAverage += i;
    }
    movAverage /= altitudeBuffer_.size();

    /**
     * We use the law of sines to get the correct angle. The tracking altitude corresponds to the ground station's
     * relative elevation with regards to the launch site altitude.
     * The distanceToLaunchSite variable is the horizontal distance between the launch site and the GS.
     */

    double beta = atan(SensorConstants::distanceToLaunchSite / abs(movAverage - SensorConstants::launchAltitude));
    double hypothenuseToLaunchSite = sqrt(
            pow(SensorConstants::distanceToLaunchSite, 2) +
            pow(SensorConstants::trackingAltitude - movAverage, 2));
    double gamma = asin(abs(movAverage - SensorConstants::launchAltitude) * sin(beta) / hypothenuseToLaunchSite);

    gamma *= 180 / M_PI;
    gamma = 123 - gamma;


    if (msecsBetween(lastTrackingAngleUpdate_, chrono::system_clock::now()) > 100) {
        lastTrackingAngleUpdate_ = chrono::system_clock::now();
        std::string toSend = std::to_string(static_cast<int>(gamma)) + "\r\n";
        cout << "angle for tracking: " << toSend;
        if (trackingEnabled_) {
            serialPort_.write(toSend);
            uint8_t carriageReturn = 0x0d;
            serialPort_.write(&carriageReturn, 1);
        }
    }

#endif
}
