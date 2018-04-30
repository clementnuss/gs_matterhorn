#ifndef GS_MATTERHORN_TELEMETRYHANDLER_H
#define GS_MATTERHORN_TELEMETRYHANDLER_H


#include <memory>
#include "IDataHandler.h"
#include "DataStructures/datastructs.h"

/**
 * The TelemetryHandler is the base class for telemetry-producing instances
 */
class TelemetryHandler : public IDataHandler<SensorsPacket> {

public:

    virtual ~TelemetryHandler() {};

    virtual void startup() = 0;

    /**
     *
     * @return The events received by this TelemetryHandler since the last call to the function
     */
    virtual std::vector<EventPacket> pollEventsData() = 0;

    /**
     *
     * @return The sensors data received by this TelemetryHandler since the last call to the function
     */
    virtual std::vector<SensorsPacket> pollSensorsData() = 0;

    /**
     *
     * @return The locations received by this TelemetryHandler since the last call to the function
     */
    virtual std::vector<GPSPacket> pollGPSData() = 0;

    virtual void sendCommand(const uint8_t *, size_t) = 0;

    virtual bool isReplayHandler()= 0;

};


#endif //GS_MATTERHORN_TELEMETRYHANDLER_H
