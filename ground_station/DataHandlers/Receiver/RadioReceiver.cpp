#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include "RadioReceiver.h"


/**
 *
 * @param portAddress
 * @param baudRate
 * @param io
 */
RadioReceiver::RadioReceiver(string portAddress)
        : byteDecoder_{}, device_{std::move(portAddress)}, serialPort_{}, thread_{},
          recvBuffer_{}, telemQueue_{100},
          bytesLogger_{LogConstants::BYTES_LOG_PATH} {

    vector<serial::PortInfo> devices_found = serial::list_ports();
    if (!devices_found.empty()) {
        std::cout << "Serial port list: " << std::endl;
        auto iter = devices_found.begin();
        while (iter != devices_found.end()) {
            serial::PortInfo device = *iter++;
            printf("(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
                   device.hardware_id.c_str());
        }
    } else {
        throw (::std::runtime_error("No Serial port available!\n"));
    }

    if (device_.empty()) {
        device_ = devices_found.front().port;
        std::cout << "The first port of the list will be used, with address: " << device_ << std::endl;
    }

    serialPort_.setBaudrate(CommunicationsConstants::TELEMETRY_BAUD_RATE);
    serialPort_.setFlowcontrol(serial::flowcontrol_none);
    serialPort_.setStopbits(serial::stopbits_one);
    serialPort_.setPort(device_);

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

void RadioReceiver::startup() {
    openSerialPort();
    thread_ = boost::thread{boost::bind(&RadioReceiver::readSerialPort, this)};
}

void RadioReceiver::openSerialPort() {

    std::cout << "Opening serial port " << device_ << " at " << CommunicationsConstants::TELEMETRY_BAUD_RATE
              << " bauds/s" << std::endl;

    try {
        serialPort_.open();
    } catch (serial::SerialException &e) {
        std::cerr << "Unable to open the serial port" << std::endl << e.what() << std::endl;
        throw (::std::runtime_error("Unable to open COM port for device " + device_ + "\n"));
    }
//    serialPort_.flush();
}

vector<TelemetryReading> RadioReceiver::pollData() {
    std::vector<TelemetryReading> telemetryBuffer{};
    telemQueue_.consume_all([&telemetryBuffer](TelemetryReading tR) { telemetryBuffer.push_back(tR); });
    return telemetryBuffer;
}

vector<RocketEvent> RadioReceiver::pollEvents() {
    std::vector<RocketEvent> eventBuffer{};
    return eventBuffer;
}

void RadioReceiver::readSerialPort() {
    size_t bytesAvailable = 0;
    bool terminate = false;
    while (!terminate) {
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
    }

}

void RadioReceiver::handleReceive(std::size_t bytesTransferred) {

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

    BytesReading bytesReading {chrono::system_clock::now(), bytes};
    vector<reference_wrapper<ILoggable>> bytesReadingVector {};
    bytesReadingVector.push_back(bytesReading);
    bytesLogger_.registerData(bytesReadingVector);

}

void RadioReceiver::unpackPayload() {
    Datagram d = byteDecoder_.retrieveDatagram();
    cout << d.sequenceNumber_ << endl;
    switch (d.payloadType_->code()) {
        case PayloadType::TELEMETRY.code(): {
            std::shared_ptr<TelemetryReading> data = std::dynamic_pointer_cast<TelemetryReading>(
                    d.deserializedPayload_);
            //TODO: make sure that the memory behaviour is correct
            telemQueue_.push(*data);
            break;
        }
        default:
            std::cout << "Wrong datagram payload type!";
            break;
    }
}


RadioReceiver::~RadioReceiver() {
    serialPort_.close();
}

