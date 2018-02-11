
#ifndef GS_MATTERHORN_STATEESTIMATOR_H
#define GS_MATTERHORN_STATEESTIMATOR_H

#include "TelemetryHandler.h"
#include "DataHandlers/Replay/TelemetryReplay.h"

#include <boost/circular_buffer.hpp>

class StateEstimator : public ITelemetryReplayHandler, public TelemetryHandler {

public:

    explicit StateEstimator(TelemetryHandler *);

    void startup() override;

    vector<EventPacket> pollEvents() override;

    vector<SensorsPacket> pollData() override;

    vector<Data3D> pollLocations() override;

    void updatePlaybackSpeed(double) override;

    void setPlaybackReversed(bool) override;

    void resetPlayback() override;

    bool endOfPlayback() override;

    bool isReplayHandler() override;

private:

    enum State {
        READY,
        BURNING_PHASE,
        BURNOUT,
        PARACHUTE_DESCENT,
        TOUCHDOWN
    };

    State currentState_ = READY;
    SensorsPacket initialTelemetryState_{};

    static constexpr int MOVING_AVERAGE_POINTS = 5;
    static constexpr double ACCELERATION_THRESHOLD = 1.5;

    boost::circular_buffer<SensorsPacket> readingsBuffer_;
    vector<EventPacket> pendingDetectedRocketEvents_;

    unique_ptr<TelemetryHandler> handler_;
    unique_ptr<ITelemetryReplayHandler> replayHandler_;

    SensorsPacket computeMA(const SensorsPacket &);

    void computeState(const SensorsPacket &);

};


#endif //GS_MATTERHORN_STATEESTIMATOR_H
