
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

private:
    boost::filesystem::path path_;
    vector<TelemetryReading> readings_;
    vector<TelemetryReading>::const_iterator lastReadingIter_;
    int64_t lastPlaybackTime_;
    double playbackSpeed_;

    void parseFile(boost::filesystem::path);

};

#endif //GS_MATTERHORN_TELEMETRYREPLAY_H
