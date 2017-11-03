#ifndef GS_MATTERHORN_RADIORECEIVER_H
#define GS_MATTERHORN_RADIORECEIVER_H

#include <DataHandlers/TelemetryHandler.h>
#include "Decoder.h"
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <FileLogger.h>

class RadioReceiver : public TelemetryHandler {

public:

    explicit RadioReceiver(std::string);

    ~RadioReceiver();

    void startup() override;

    vector<TelemetryReading> pollData() override;

    vector<RocketEvent> pollEvents() override;

private:

    void asyncRead();

    void openSerialPort();

    void handleReceive(const boost::system::error_code &error, std::size_t);

    void unpackPayload();

    Decoder byteDecoder_;
    std::string device_;
    boost::asio::io_service ioService_;
    boost::asio::serial_port serialPort_;
    boost::thread thread_;
    boost::array<uint8_t, 500> recvBuffer_;
    boost::lockfree::spsc_queue<TelemetryReading> telemQueue_;


};


#endif //GS_MATTERHORN_RADIORECEIVER_H
