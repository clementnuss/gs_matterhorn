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
    //TODO: move port and baud rate from derived class to here
    virtual void startup() = 0;

    /**
     *
     * @return The events received by this TelemetryHandler since the last call to the function
     */
    virtual std::vector<EventPacket> pollEvents() = 0;

    /**
     *
     * @return The locations received by this TelemetryHandler since the last call to the function
     */
    virtual std::vector<Data3D> pollLocations() = 0;

    virtual bool isReplayHandler()= 0;

};


#endif //GS_MATTERHORN_TELEMETRYHANDLER_H
