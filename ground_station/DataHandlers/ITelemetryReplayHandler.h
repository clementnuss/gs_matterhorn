#ifndef GS_MATTERHORN_TELEMETRY_REPLAY_HANDLER_H
#define GS_MATTERHORN_TELEMETRY_REPLAY_HANDLER_H


#include "TelemetryHandler.h"

class ITelemetryReplayHandler {

public:

    virtual void updatePlaybackSpeed(double)= 0;

    virtual void setPlaybackReversed(bool)= 0;

    virtual void resetPlayback()= 0;

    virtual bool endOfPlayback()= 0;

};


#endif //GS_MATTERHORN_TELEMETRY_REPLAY_HANDLER_H
