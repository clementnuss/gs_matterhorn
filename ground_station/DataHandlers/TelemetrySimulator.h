
#ifndef GS_MATTERHORN_TELEMETRYSIMULATOR_H
#define GS_MATTERHORN_TELEMETRYSIMULATOR_H

#include "TelemetryHandler.h"

using namespace std;

class TelemetrySimulator : public TelemetryHandler {

public:
    TelemetrySimulator();
    virtual vector<TelemetryReading> getData() override;

private:
    const TelemetryReading generateTelemetry();

    const vector<TelemetryReading> generateTelemetryVector();
};


#endif //GS_MATTERHORN_TELEMETRYSIMULATOR_H
