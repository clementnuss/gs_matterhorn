
#ifndef GS_MATTERHORN_STATEESTIMATOR_H
#define GS_MATTERHORN_STATEESTIMATOR_H

#include "TelemetryHandler.h"
#include "TelemetryReplay.h"

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

    unique_ptr<TelemetryHandler> handler_;
    unique_ptr<TelemetryReplayHandler> replayHandler_;

    bool isReplayHandler();
};


#endif //GS_MATTERHORN_STATEESTIMATOR_H
