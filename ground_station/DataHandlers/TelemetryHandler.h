#ifndef GS_MATTERHORN_TELEMETRYHANDLER_H
#define GS_MATTERHORN_TELEMETRYHANDLER_H


#include <memory>
#include "IDataHandler.h"
#include "DataStructures/datastructs.h"

/**
 * The TelemetryHandler is the base class for telemetry-producing instances
 */
class TelemetryHandler : public IDataHandler<TelemetryReading> {

public:
    //TODO: move port and baud rate from derived class to here
    virtual void startup() = 0;

    virtual std::vector<RocketEvent> pollEvents() = 0;
};


#endif //GS_MATTERHORN_TELEMETRYHANDLER_H
