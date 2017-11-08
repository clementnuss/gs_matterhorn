
#ifndef GS_MATTERHORN_STATEESTIMATOR_H
#define GS_MATTERHORN_STATEESTIMATOR_H

#include "TelemetryHandler.h"
#include "TelemetryReplay.h"

#include <boost/circular_buffer.hpp>

class StateEstimator : public TelemetryReplayHandler, public TelemetryHandler {

public:

    explicit StateEstimator(TelemetryHandler *);

    void startup() override;

    vector<RocketEvent> pollEvents() override;

    vector<TelemetryReading> pollData() override;

    void updatePlaybackSpeed(double) override;

    void setPlaybackReversed(bool) override;

    void resetPlayback() override;

    bool endOfPlayback() override;

private:

    static constexpr int MOVING_AVERAGE_POINTS = 5;

    boost::circular_buffer<TelemetryReading> readingsBuffer_;

    unique_ptr<TelemetryHandler> handler_;
    unique_ptr<TelemetryReplayHandler> replayHandler_;

    bool isReplayHandler();

    TelemetryReading computeMA(const TelemetryReading &);
};


#endif //GS_MATTERHORN_STATEESTIMATOR_H
