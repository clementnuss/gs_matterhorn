
#ifndef GS_MATTERHORN_TELEMETRYSIMULATOR_H
#define GS_MATTERHORN_TELEMETRYSIMULATOR_H

#include "TelemetryHandler.h"
#include <QTime>

using namespace std;

class TelemetrySimulator : public TelemetryHandler {

public:
    TelemetrySimulator();

    void startup() override;

    virtual vector<RocketEvent> pollEvents() override;

    virtual vector<TelemetryReading> pollData() override;

private:
    RocketEvent generateEvent();

    QTime time;
    const TelemetryReading generateTelemetry();
    const vector<TelemetryReading> generateTelemetryVector();
};


#endif //GS_MATTERHORN_TELEMETRYSIMULATOR_H
