#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include "RadioReceiver.h"


/**
 *
 * @param device
 * @param baudRate
 * @param io
 */
RadioReceiver::RadioReceiver(string device)
        : byteDecoder_{}, device_{std::move(device)}, ioService_{}, thread_{},
          serialPort_{ioService_}, telemQueue_{100} {}

void RadioReceiver::startup() {
    openSerialPort();
    thread_ = boost::thread{boost::bind(&boost::asio::io_service::run, &ioService_)};
    asyncRead();
}

void RadioReceiver::openSerialPort() {
    std::cout << "Opening serial port " << device_ << " at " << CommunicationsConstants::TELEMETRY_BAUD_RATE
              << " bauds/s" << std::endl;
    try {
        serialPort_.open(device_);
    } catch (boost::system::system_error &e) {
        std::cerr << "Unable to open the serial port" << std::endl << boost::diagnostic_information(e);
        throw (::std::runtime_error("Unable to open COM port " + device_ + "\n"));
    }

    serialPort_.set_option(boost::asio::serial_port_base::baud_rate(CommunicationsConstants::TELEMETRY_BAUD_RATE));
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

void RadioReceiver::asyncRead() {

    serialPort_.async_read_some(
            boost::asio::buffer(recvBuffer_),
            boost::bind(&RadioReceiver::handleReceive,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

}

void RadioReceiver::handleReceive(const boost::system::error_code &error,
                                  std::size_t bytesTransferred) {

    if (!error) {
        for (int i = 0; i < bytesTransferred; ++i) {
            //TODO: log every byte received

#ifdef DEBUG
            std::bitset<8> x(recvBuffer_[i]);
            cout << x << ' ' << std::flush;
#endif
            if (byteDecoder_.processByte(recvBuffer_[i])) {
                unpackPayload();
            }
        }

        cout << std::endl << std::endl;

        asyncRead();
    } else {
        cout << "Problem while communicating with the serial port\n" << error.message() << endl;
    }

}

void RadioReceiver::unpackPayload() {
    Datagram d = byteDecoder_.retrieveDatagram();
    cout << d.sequenceNumber_ << endl;
    switch (d.payloadType_) {
        case DatagramPayloadType::TELEMETRY: {
            std::shared_ptr<TelemetryReading> data = std::dynamic_pointer_cast<TelemetryReading>(
                    d.deserializedPayload_);
            //TODO: make sure that the memory behaviour is correct
            telemQueue_.push(*data);
            break;
        }
        case DatagramPayloadType::Count:
            std::cout << "Wrong datagram payload type!";
            break;
    }
}


RadioReceiver::~RadioReceiver() {
    serialPort_.close();
}

