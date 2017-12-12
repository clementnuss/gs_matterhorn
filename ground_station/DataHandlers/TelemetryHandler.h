#ifndef GS_MATTERHORN_TELEMETRYHANDLER_H
#define GS_MATTERHORN_TELEMETRYHANDLER_H


#include <memory>
#include "IDataHandler.h"
#include "DataStructures/datastructs.h"


class TelemetryHandler : public IDataHandler<TelemetryReading> {

public:
    //TODO: move port and baud rate from derived class to here
    virtual void startup() = 0;

    /**
     *
     * @return The events received by this TelemetryHandler since the last call to the function
     */
    virtual std::vector<RocketEvent> pollEvents() = 0;

    /**
     *
     * @return The locations received by this TelemetryHandler since the last call to the function
     */
    virtual std::vector<XYZReading> pollLocations() = 0;
};


#endif //GS_MATTERHORN_TELEMETRYHANDLER_H
