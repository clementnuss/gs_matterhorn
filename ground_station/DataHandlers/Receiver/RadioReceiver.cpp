#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/bind.hpp>
#include "RadioReceiver.h"


/**
 *
 * @param device
 * @param baudRate
 * @param io
 */
RadioReceiver::RadioReceiver(boost::asio::io_service &io)
        : byteDecoder_{}, serialPort_{io} {}

void RadioReceiver::openSerialPort(const string device, const uint32_t baudRate) {
    std::cout << "Opening serial port " << device << " at " << baudRate << " bauds/s" << std::endl;
    try {
        serialPort_.open(device);
    } catch (boost::system::system_error &e) {
        std::cerr << "Unable to open the serial port" << std::endl << boost::diagnostic_information(e);
        throw (::std::runtime_error("Unable to open COM port " + device + "\n"));
    }

    serialPort_.set_option(boost::asio::serial_port_base::baud_rate(baudRate));
}

vector<TelemetryReading> RadioReceiver::getData() {
    return TelemetryHandler::getData();
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
        byteDecoder_.processByte(recvBuffer_[i]);
    }

    asyncRead();
}


RadioReceiver::~RadioReceiver() {
    serialPort_.close();
}

