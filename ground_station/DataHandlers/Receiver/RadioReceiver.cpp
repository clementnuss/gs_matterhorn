#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <thread>
#include "RadioReceiver.h"


/**
 *
 * @param portAddress
 * @param baudRate
 * @param io
 */
RadioReceiver::RadioReceiver(const string &hardwareID, const string &logTitle)
        : byteDecoder_{logTitle}, devicePort_{}, serialPort_{}, thread_{},
          recvBuffer_{}, sensorsDataQueue_{100}, eventsDataQueue_{100}, controlDataQueue_{100}, gpsDataQueue_{100},
          bytesLogger_{LogConstants::BYTES_LOG_PATH + logTitle} {

    if (hardwareID.empty()) {
        std::cerr << "Empty serial port specified, telemetry acquisition will not work." << std::endl;
        return;
    }

    vector<serial::PortInfo> devices_found = serial::list_ports();
    if (!devices_found.empty()) {
//        std::cout << "Serial port list: " << std::endl;
        auto iter = devices_found.begin();
        while (iter != devices_found.end()) {
            serial::PortInfo device = *iter++;
            printf("(%s, %s, %s)\n",
                   device.port.c_str(), device.description.c_str(), device.hardware_id.c_str());
            if (device.hardware_id == hardwareID) {
                devicePort_ = device.port;
            }
        }

        if (devicePort_.empty()) {
            throw std::invalid_argument("Could not find serial port with hardware id: " + hardwareID);
        }
    } else {
        std::cerr << "No Serial port available, telemetry acquisition will not work." << std::endl;
    }


    serialPort_.setBaudrate(CommunicationsConstants::TELEMETRY_BAUD_RATE);
    serialPort_.setFlowcontrol(serial::flowcontrol_none);
    serialPort_.setStopbits(serial::stopbits_one);
    serialPort_.setPort(devicePort_);

    /*
     * Timeouts in [ms]
     * uint32_t inter_byte_timeout, uint32_t read_timeout_constant, uint32_t read_timeout_multiplier,
     * uint32_t write_timeout_constant, uint32_t write_timeout_multiplier
     * At 38400 Bauds, 1 byte is transmitted in ~140us.
     *
     */
    serialPort_.setTimeout(1, 5, 0, 5, 0);
    recvBuffer_ = new uint8_t[BUFFER_SIZE];
}

void
RadioReceiver::startup() {
    openSerialPort();
    thread_ = boost::thread{boost::bind(&RadioReceiver::readSerialPort, this)};
}

void
RadioReceiver::openSerialPort() {

    std::cout << "Opening serial port " << devicePort_ << " at " << CommunicationsConstants::TELEMETRY_BAUD_RATE
              << " bauds/s" << std::endl;

    try {
        serialPort_.open();
    } catch (serial::SerialException &e) {
        throw (std::runtime_error("Unable to open COM port for device " + devicePort_ + "\n" + e.what()));
    } catch (std::invalid_argument &e) {
        throw (std::runtime_error("Unable to open COM port, invalid argument\n"));
    } catch (serial::IOException &e) {
        throw (std::runtime_error("Unable to open COM port, invalid argument\n"));
    }
//    serialPort_.flush();
}

template<class S>
static std::vector<S>
consumeQueue(boost::lockfree::spsc_queue<S> *queue) {
    std::vector<S> recipient{};
    queue->consume_all([&recipient](S s) { recipient.push_back(s); });
    return recipient;
};

std::vector<SensorsPacket>
RadioReceiver::pollSensorsData() {
    return consumeQueue(&sensorsDataQueue_);
}

std::vector<EventPacket>
RadioReceiver::pollEventsData() {
    return consumeQueue(&eventsDataQueue_);
}

std::vector<GPSPacket>
RadioReceiver::pollGPSData() {
    return consumeQueue(&gpsDataQueue_);
}

void
RadioReceiver::readSerialPort() {
    size_t bytesAvailable = 0;
    bool terminate = false;
    while (!terminate) {
        try {
            if ((bytesAvailable = serialPort_.available()) == 0) {
                std::this_thread::sleep_for(chrono::milliseconds(7));
                continue;
            }
            if (bytesAvailable > BUFFER_SIZE) {
                bytesAvailable = BUFFER_SIZE; // We do not want to overflow the buffer.
                //TODO: Display warning message
            }

            size_t bytesRead = serialPort_.read(recvBuffer_, bytesAvailable);
            handleReceive(bytesRead);
        } catch (const serial::IOException &e) {
            std::cerr << "IOException while reading serial port " << devicePort_ << std::endl;
            terminate = true;
        } catch (const serial::SerialException &e) {
            std::cerr << "SerialException while reading serial port" << devicePort_ << std::endl;
        }
    }

}

void
RadioReceiver::handleReceive(std::size_t bytesTransferred) {

    vector<uint8_t> bytes{};

    for (int i = 0; i < bytesTransferred; ++i) {
        //TODO: log every byte received
        bytes.push_back(recvBuffer_[i]);
#if DEBUG
        std::bitset<8> x(recvBuffer_[i]);
        cout << x << ' ' << std::flush;
#endif
        if (byteDecoder_.processByte(recvBuffer_[i])) {
            unpackPayload();
        }
    }

    BytesReading bytesReading{chrono::system_clock::now(), bytes};
    vector<reference_wrapper<ILoggable>> bytesReadingVector{};
    bytesReadingVector.emplace_back(bytesReading);
    bytesLogger_.registerData(bytesReadingVector);

}

void
RadioReceiver::unpackPayload() {
    Datagram d = byteDecoder_.retrieveDatagram();

    switch (d.payloadType_->code()) {
        case CommunicationsConstants::TELEMETRY_TYPE:
            sensorsDataQueue_.push(PayloadDataConverter::toSensorsPacket(d.payloadData_));
            break;
        case CommunicationsConstants::EVENT_TYPE:
            eventsDataQueue_.push(PayloadDataConverter::toEventPacket(d.payloadData_));
            break;
        case CommunicationsConstants::CONTROL_TYPE:
            controlDataQueue_.push(PayloadDataConverter::toControlPacket(d.payloadData_));
            break;
        case CommunicationsConstants::GPS_TYPE:
            gpsDataQueue_.push(PayloadDataConverter::toGPSPacket(d.payloadData_));
            break;
        case CommunicationsConstants::TELEMETRY_ERT18_TYPE:
            sensorsDataQueue_.push(PayloadDataConverter::toERT18SensorsPacket(d.payloadData_));
        default:
            break;
    }
}


RadioReceiver::~RadioReceiver() {
    std::cout << "Closing radio receiver" << std::endl;
    serialPort_.close();
}

bool
RadioReceiver::isReplayHandler() {
    return false;
}

void
RadioReceiver::sendCommand(const uint8_t *command, size_t size) {
    serialPort_.write(command, size);
}
