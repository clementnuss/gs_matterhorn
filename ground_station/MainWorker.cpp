#include <QThread>
#include <iostream>
#include <DataHandlers/Receiver/RadioReceiver.h>
#include <DataHandlers/Replay/TelemetryReplay.h>
#include <ConfigParser/ConfigParser.h>
#include <DataHandlers/Receiver/CompositeReceiver.h>
#include "MainWorker.h"
#include "PacketDispatcher.h"


/**
 *
 * @param telemetryHandler
 */
Worker::Worker(GSMainwindow *gsMainwindow) :
        packetDispatcher_{new PacketDispatcher(this)},
        loggingEnabled_{false},
        lastIteration_{chrono::system_clock::now()},
        timeOfLastLinkCheck_{chrono::system_clock::now()},
        timeOfLastReceivedTelemetry_{chrono::system_clock::now()},
        millisBetweenLastTwoPackets_{0},
        replayMode_{false},
        updateHandler_{false},
        telemetryHandler900MHz_{},
        telemetryHandler433MHz_{},
        compositeReceiver_{} {

    gsMainwindow->setRealTimeMode();
    try {
        // Xbee FTDIBUS\COMPORT&VID_0403&PID_6015
        // Serial USB\VID_067B&PID_2303&REV_0300
        telemetryHandler900MHz_ = std::make_unique<RadioReceiver>(ConfSingleton::instance().get("receiverID.900MHz", std::string{""}), "_900Mhz");
        telemetryHandler900MHz_->startup();
    } catch (const std::runtime_error &e) {
        std::cerr << "Unable to start radio receiver handler:\n" << e.what();
    }

    try {
        // Adafruit USB\VID_1A86&PID_7523&REV_0263
        telemetryHandler433MHz_ = std::make_unique<RadioReceiver>(ConfSingleton::instance().get("receiverID.433MHz", std::string{""}), "_433MHz");
        telemetryHandler433MHz_->startup();
    } catch (const std::runtime_error &e) {
        std::cerr << "Unable to start radio receiver handler:\n" << e.what();
    }

    compositeReceiver_ = std::make_unique<CompositeReceiver>(std::move(telemetryHandler900MHz_), std::move(telemetryHandler433MHz_), 10);

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
    std::cout << "Destroying worker instance" << std::endl;
    delete packetDispatcher_;
}

/**
 * Emits all statuses boolean. This should be used once the UI has loaded to initialise
 * the different status color markers.
 */
void
Worker::emitAllStatuses() {
    emit loggingStatusChanged(loggingEnabled_);
}

/**
 * Entry point of the executing thread
 */
void
Worker::run() {

    while (!(QThread::currentThread()->isInterruptionRequested())) {
        mainRoutine();
    }

    std::cout << "The worker thread was interrupted" << std::endl;
}

/**
 * Work loop
 */
void
Worker::mainRoutine() {
    //TODO: adapt sleep time so as to have proper framerate
    auto elapsed = msecsBetween(lastIteration_, chrono::system_clock::now());
    if (elapsed < UIConstants::REFRESH_RATE) {
        QThread::msleep(UIConstants::REFRESH_RATE - static_cast<unsigned long>(elapsed));
    }
    lastIteration_ = chrono::system_clock::now();
    checkLinkStatuses();

    std::list<std::unique_ptr<DataPacket>> dataPackets = compositeReceiver_->pollData();
    for (std::unique_ptr<DataPacket> &dp : dataPackets) {
        dp.release()->dispatchWith(packetDispatcher_);
    }

    std::list<std::unique_ptr<ATCommandResponse>> atResponses = compositeReceiver_->pollATResponses();
    for (std::unique_ptr<ATCommandResponse> &cr : atResponses) {
        cr.release()->dispatchWith(packetDispatcher_);
    }

    packetDispatcher_->processDataFlows();

    QCoreApplication::sendPostedEvents(this);
    QCoreApplication::processEvents();
}


/**
 * Emits a boolean to the UI indicating the current status of the logger.
 */
void
Worker::updateLoggingStatus() {
    emit loggingStatusChanged(packetDispatcher_->toggleLogging());
}


void
Worker::toggleTracking() {
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
void
Worker::checkLinkStatuses() {
    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(timeOfLastLinkCheck_, now);

    if (elapsedMillis > CommunicationsConstants::MSECS_BETWEEN_LINK_CHECKS) {

        timeOfLastLinkCheck_ = now;
        emit ppsOnPrimaryRFChanged(compositeReceiver_->getPPS(true));
        emit ppsOnSecondaryRFChanged(compositeReceiver_->getPPS(false));
        compositeReceiver_->sendCommand(&ATCommandResponse::RSSI_COMMAND_RF1[0], ATCommandResponse::RSSI_COMMAND_RF1.size(), true);
        compositeReceiver_->sendCommand(&ATCommandResponse::RSSI_COMMAND_RF2[0], ATCommandResponse::RSSI_COMMAND_RF2.size(), false);
    }
}


void
Worker::updatePlaybackSpeed(double newSpeed) {
    assert(replayMode_);
    //auto *telemReplay = dynamic_cast<ITelemetryReplayHandler *>(telemetryHandler900MHz_.get());
    //telemReplay->updatePlaybackSpeed(newSpeed);
}

void
Worker::resetPlayback() {
    assert(replayMode_);
    //auto *telemReplay = dynamic_cast<ITelemetryReplayHandler *>(telemetryHandler900MHz_.get());
    //telemReplay->resetPlayback();
}

void
Worker::reversePlayback(bool reversed) {
    assert(replayMode_);
    //auto *telemReplay = dynamic_cast<ITelemetryReplayHandler *>(telemetryHandler900MHz_.get());
    //telemReplay->setPlaybackReversed(reversed);
}

void
Worker::transmitCommand(int command) {
    if (command == 14) {
        uint8_t xBeeCommand[] = {0x7E, 0x00, 0x1B, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
                                 0xFE, 0x00, 0x43, 0x55, 0x55, 0x55, 0x55, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x14, 0x35};
        compositeReceiver_->sendCommand(xBeeCommand, sizeof(xBeeCommand), true);
    } else if (command == 22) {
        uint8_t xBeeCommand[] = {0x7E, 0x00, 0x1B, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
                                 0xFE, 0x00, 0x43, 0x55, 0x55, 0x55, 0x55, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x22, 0x19};
        compositeReceiver_->sendCommand(xBeeCommand, sizeof(xBeeCommand), true);
    }
}


void
Worker::defineReplayMode(const QString &parameters) {
    /* IReceiver *handler = nullptr;
     try {
         handler = new TelemetryReplay(parameters.toStdString());
         handler->startup();
     } catch (std::runtime_error &e) {
         std::cerr << "Error when starting replay handler:\n" << e.what();
     }
     newHandler_ = unique_ptr<IReceiver>{handler};
     updateHandler_.store(true);
     */
}


void
Worker::defineRealtimeMode(const QString &parameters) {
    /*IReceiver *handler = nullptr;
    try {
        handler = new RadioReceiver(parameters.toStdString(), "");
        handler->startup();
    } catch (std::runtime_error &e) {
        std::cerr << "Error when starting RadioReceiver handler:\n" << e.what();
    }
    newHandler_ = unique_ptr<IReceiver>{handler};
    updateHandler_.store(true);
     */
}

void
Worker::moveTrackingSystem(double currentAltitude) {
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


