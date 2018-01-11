
#ifndef GS_MATTERHORN_TELEMETRYSIMULATOR_H
#define GS_MATTERHORN_TELEMETRYSIMULATOR_H

#include "DataHandlers/TelemetryHandler.h"
#include <chrono>

using namespace std;

/**
 * Telemetry Simulator can be used in place of a TelemetryHandler to generate dummy telemetry data
 * for testing purposes.
 */
class TelemetrySimulator : public TelemetryHandler {

public:
    TelemetrySimulator();

    void startup() override;

    vector<RocketEvent> pollEvents() override;

    vector<TelemetryReading> pollData() override;

    bool isReplayHandler() override;

    vector<XYZReading> pollLocations() override;

    void setVariableRate(bool);

private:

    RocketEvent generateEvent();

    void updateHandlerStatus();

    static constexpr double VARIABLE_RATE_TIME_MULTIPLIER = 2.0 * M_PI * 0.05;

    const TelemetryReading generateTelemetry();

    const vector<TelemetryReading> generateTelemetryVector();

    HandlerStatus simulatorStatus;
    uint32_t sequenceNumber_;
    chrono::system_clock::time_point timeOfLastPolledData;
    chrono::system_clock::time_point timeOfLastPolledGeoData;
    bool geoDataTriggered_;
    chrono::system_clock::time_point timeOfInitialization;
    bool variableRate;
};


#endif //GS_MATTERHORN_TELEMETRYSIMULATOR_H
