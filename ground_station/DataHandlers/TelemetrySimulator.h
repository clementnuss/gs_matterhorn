
#ifndef GS_MATTERHORN_TELEMETRYSIMULATOR_H
#define GS_MATTERHORN_TELEMETRYSIMULATOR_H

#include "TelemetryHandler.h"

using namespace std;

class TelemetrySimulator : public TelemetryHandler {

public:
    TelemetrySimulator();

    static constexpr size_t MAX_RANDOM_VECTOR_LENGTH = 16;

    virtual vector<TelemetryReading> getData() override;

private:
    const TelemetryReading generateTelemetry();

    const vector<TelemetryReading> generateTelemetryVector();
};


#endif //GS_MATTERHORN_TELEMETRYSIMULATOR_H
