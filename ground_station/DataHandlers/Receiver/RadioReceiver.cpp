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
        : byteDecoder_{}, device_{device}, ioService_{}, thread_{}, serialPort_{ioService_} {}

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

}

vector<RocketEvent> RadioReceiver::pollEvents() {

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

    for (int i = 0; i < bytesTransferred; ++i) {
        //TODO: log every byte received
        cout << recvBuffer_[i];
        /*if(byteDecoder_.processByte(recvBuffer_[i])){
            Datagram d = byteDecoder_.retrieveDatagram();
            cout << d.sequenceNumber_ << endl;
        }*/
    }

    asyncRead();
}


RadioReceiver::~RadioReceiver() {
    serialPort_.close();
}

