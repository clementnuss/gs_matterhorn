#include <iostream>
#include <unistd.h>

#include <DataHandlers/Receiver/RadioReceiver.h>
#include <DataHandlers/Replay/TelemetryReplay.h>
#include <ConfigParser/ConfigParser.h>
#include <DataHandlers/Receiver/CompositeReceiver.h>
#include <Utilities/TimeUtils.h>
#include "MainWorker.h"
#include "PacketDispatcher.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

/**
 *
 * @param telemetryHandler
 */
Worker::Worker() :
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

    try {
        string port = ConfSingleton::instance().get("receiverID.900MHz.port", std::string{""});
        if (!port.empty()) {
            telemetryHandler900MHz_ = std::make_unique<RadioReceiver>("", port, "_900Mhz");
        } else {
            string hardwareID = ConfSingleton::instance().get("receiverID.900MHz.hardwareID", std::string{""});
            telemetryHandler900MHz_ = std::make_unique<RadioReceiver>(hardwareID, "", "_900Mhz");
        }
        telemetryHandler900MHz_->startup();
    } catch (const std::runtime_error &e) {
        std::cerr << "Unable to start 900MHz radio receiver handler:\n" << e.what();
    }

    try {
        string port = ConfSingleton::instance().get("receiverID.433MHz.port", std::string{""});
        if (!port.empty()) {
            telemetryHandler433MHz_ = std::make_unique<RadioReceiver>("", port, "_433Mhz");
        } else {
            string hardwareID = ConfSingleton::instance().get("receiverID.433MHz.hardwareID", std::string{""});
            telemetryHandler433MHz_ = std::make_unique<RadioReceiver>(hardwareID, "", "_433Mhz");
        }
        telemetryHandler433MHz_->startup();
    } catch (const std::runtime_error &e) {
        std::cerr << "Unable to start 433MHz radio receiver handler:\n" << e.what();
    }

    compositeReceiver_ = std::make_unique<CompositeReceiver>(std::move(telemetryHandler900MHz_),
                                                             std::move(telemetryHandler433MHz_), 10);

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
 * Entry point of the executing thread
 */
void
Worker::run() {

    for (;;) {
        mainRoutine();
    }
}

/**
 * Work loop
 */
void
Worker::mainRoutine() {
    //TODO: adapt sleep time so as to have proper framerate
    auto elapsed = msecsBetween(lastIteration_, chrono::system_clock::now());
    if (elapsed < UIConstants::REFRESH_RATE) {
        usleep(1000 * (UIConstants::REFRESH_RATE - static_cast<unsigned long>(elapsed)));
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
}

/*
 * Determine the status of the communication based on the quantity of data received during the past second.
 * Sends a request to so that the receiver sends the RSSI of the last received packet
 */
void
Worker::checkLinkStatuses() {
    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long elapsedMillis = msecsBetween(timeOfLastLinkCheck_, now);

    if (elapsedMillis > CommunicationsConstants::MSECS_BETWEEN_LINK_CHECKS) {

        timeOfLastLinkCheck_ = now;
        //emit ppsOnPrimaryRFChanged(compositeReceiver_->getPPS(true));
        //emit ppsOnSecondaryRFChanged(compositeReceiver_->getPPS(false));
        compositeReceiver_->sendCommand(&ATCommandResponse::RSSI_COMMAND_RF1[0],
                                        ATCommandResponse::RSSI_COMMAND_RF1.size(), true);
        compositeReceiver_->sendCommand(&ATCommandResponse::RSSI_COMMAND_RF2[0],
                                        ATCommandResponse::RSSI_COMMAND_RF2.size(), false);
    }
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


#pragma clang diagnostic pop