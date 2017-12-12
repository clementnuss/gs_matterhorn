#ifndef GS_MATTERHORN_RADIORECEIVER_H
#define GS_MATTERHORN_RADIORECEIVER_H

#include <DataHandlers/TelemetryHandler.h>
#include "Decoder.h"
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <FileLogger.h>
#include <serial/serial.h>

/**
 * A telemetry handler which receives data by the mean of a serial port connected to
 * a transciever
 */
class RadioReceiver : public TelemetryHandler {

    static constexpr uint32_t BUFFER_SIZE = 4096;

public:

    explicit RadioReceiver(std::string);

    ~RadioReceiver() override;

    void startup() override;

    vector<TelemetryReading> pollData() override;

    vector<RocketEvent> pollEvents() override;

private:

    /**
     * Reads all the bytes available on the serial port's buffer and forwards it to
     * the Decoder
     */
    void readSerialPort();

    void openSerialPort();

    void handleReceive(std::size_t);

    void unpackPayload();

    Decoder byteDecoder_;
    std::string device_;
    serial::Serial serialPort_;
    boost::thread thread_;
    uint8_t* recvBuffer_;
    boost::lockfree::spsc_queue<TelemetryReading> telemQueue_;


};


#endif //GS_MATTERHORN_RADIORECEIVER_H
