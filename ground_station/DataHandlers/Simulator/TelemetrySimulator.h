
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

    vector<EventPacket> pollEvents() override;

    vector<SensorsPacket> pollData() override;

    bool isReplayHandler() override;

    vector<Data3D> pollLocations() override;

    void setVariableRate(bool);

private:

    EventPacket generateEvent();

    void updateHandlerStatus();

    static constexpr double VARIABLE_RATE_TIME_MULTIPLIER = 2.0 * M_PI * 0.05;

    const SensorsPacket generateTelemetry();

    const vector<SensorsPacket> generateTelemetryVector();

    HandlerStatus simulatorStatus;
    uint32_t sequenceNumber_;
    chrono::system_clock::time_point timeOfLastPolledData;
    chrono::system_clock::time_point timeOfLastPolledGeoData;
    bool geoDataTriggered_;
    chrono::system_clock::time_point timeOfInitialization;
    bool variableRate;
};


#endif //GS_MATTERHORN_TELEMETRYSIMULATOR_H
