#ifndef GS_MATTERHORN_RADIORECEIVER_H
#define GS_MATTERHORN_RADIORECEIVER_H


#define SERIAL_BAUD_RATE 57600

#include <DataHandlers/TelemetryHandler.h>
#include "Decoder.h"
#include <../lib/boost-cmake/boost/boost_1_64_0/boost/asio.hpp>
#include <../lib/boost-cmake/boost/boost_1_64_0/boost/array.hpp>

class RadioReceiver : public TelemetryHandler {

public:

    explicit RadioReceiver(boost::asio::io_service &);
    ~RadioReceiver();

    void openSerialPort(string device, uint32_t baudRate);

    vector<TelemetryReading> getData() override;

private:

    void asyncRead();

    void handleReceive(const boost::system::error_code &error, std::size_t);

    Decoder byteDecoder_;
    boost::asio::serial_port serialPort_;
    boost::array<uint8_t, 500> recvBuffer_;
    vector<TelemetryReading> telemetryBuffer;

};


#endif //GS_MATTERHORN_RADIORECEIVER_H
