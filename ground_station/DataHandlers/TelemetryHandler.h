#ifndef GS_MATTERHORN_TELEMETRYHANDLER_H
#define GS_MATTERHORN_TELEMETRYHANDLER_H


#include <c++/memory>
#include "IDataHandler.h"
#include "DataStructures/datastructs.h"


class TelemetryHandler : public IDataHandler<TelemetryReading> {

public:
    virtual std::vector<TelemetryReading> getData() override;

    virtual std::vector<RocketEvent> getEvents();
};


#endif //GS_MATTERHORN_TELEMETRYHANDLER_H
