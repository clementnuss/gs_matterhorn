
#ifndef GS_MATTERHORN_TELEMETRYREPLAY_H
#define GS_MATTERHORN_TELEMETRYREPLAY_H

#include <boost/filesystem.hpp>
#include <chrono>
#include "TelemetryHandler.h"

class TelemetryReplay : public TelemetryHandler {

public:

    explicit TelemetryReplay(std::string &);

    void startup() override;

    vector<RocketEvent> pollEvents() override;

    vector<TelemetryReading> pollData() override;

    void updatePlaybackSpeed(double);

    void setPlaybackReversed(bool);

    void resetPlayback();

    bool endOfPlayback();

private:
    boost::filesystem::path path_;
    vector<TelemetryReading> readings_;
    vector<TelemetryReading>::const_iterator endReadingsIter_;
    vector<TelemetryReading>::const_iterator frontReadingsIter_;
    chrono::system_clock::time_point lastPlaybackTime_;
    uint32_t lastTimeStamp_;
    double playbackSpeed_;
    bool playbackReversed_;

    void parseFile(boost::filesystem::path);

};

#endif //GS_MATTERHORN_TELEMETRYREPLAY_H
