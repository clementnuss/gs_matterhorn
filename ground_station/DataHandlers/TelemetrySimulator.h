
#ifndef GS_MATTERHORN_TELEMETRYSIMULATOR_H
#define GS_MATTERHORN_TELEMETRYSIMULATOR_H

#include "TelemetryHandler.h"
#include <QTime>
#include <chrono>

using namespace std;

class TelemetrySimulator : public TelemetryHandler {

public:
    TelemetrySimulator();

    void startup() override;

    vector<RocketEvent> pollEvents() override;

    vector<TelemetryReading> pollData() override;

    void setVariableRate(bool);
private:

    RocketEvent generateEvent();

    void updateHandlerStatus();

    static constexpr double VARIABLE_RATE_TIME_MULTIPLIER = 2.0 * M_PI * 0.05;
    const TelemetryReading generateTelemetry();
    const vector<TelemetryReading> generateTelemetryVector();

    chrono::system_clock::time_point startTime_;
    uint32_t sequenceNumber_;
    HandlerStatus simulatorStatus;
    chrono::system_clock::time_point timeOfLastPolledData;
    bool variableRate;
};


#endif //GS_MATTERHORN_TELEMETRYSIMULATOR_H
