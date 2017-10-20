#ifndef GS_MATTERHORN_RADIORECEIVER_H
#define GS_MATTERHORN_RADIORECEIVER_H


#define SERIAL_BAUD_RATE 57600

#include <DataHandlers/TelemetryHandler.h>
#include <Boost/asio.hpp>
#include <Boost/array.hpp>

class RadioReceiver : public TelemetryHandler {

public:
    explicit RadioReceiver(boost::asio::io_service &);

    void openSerialPort(string device, uint32_t baudRate);

    ~RadioReceiver();

    vector<TelemetryReading> getData() override;

    void asyncRead();

private:

    boost::asio::serial_port serialPort_;
    boost::array<uint8_t, 20> recvBuffer_;
    vector<TelemetryReading> telemetryBuffer;

    void handleReceive(const boost::system::error_code &error, std::size_t);

};


#endif //GS_MATTERHORN_RADIORECEIVER_H
