
#ifndef GS_MATTERHORN_TELEMETRYREPLAY_H
#define GS_MATTERHORN_TELEMETRYREPLAY_H

#include <boost/filesystem.hpp>
#include <chrono>
#include "DataHandlers/TelemetryHandler.h"
#include "ITelemetryReplayHandler.h"

class TelemetryReplay : public TelemetryHandler, public ITelemetryReplayHandler {

public:

    explicit TelemetryReplay(const string &);

    void startup() override;

    vector<EventPacket> pollEventsData() override;

    vector<SensorsPacket> pollSensorsData() override;

    vector<GPSPacket> pollGPSData() override;

    void updatePlaybackSpeed(double) override;

    void setPlaybackReversed(bool) override;

    void resetPlayback() override;

    bool endOfPlayback() override;

    bool isReplayHandler() override;

private:
    boost::filesystem::path path_;
    vector<SensorsPacket> readings_;
    vector<SensorsPacket>::const_iterator endReadingsIter_;
    vector<SensorsPacket>::const_iterator frontReadingsIter_;
    chrono::system_clock::time_point lastPlaybackTime_;
    uint32_t lastTimeStamp_;
    double playbackSpeed_;
    bool playbackReversed_;

    void parseFile(boost::filesystem::path);

};

#endif //GS_MATTERHORN_TELEMETRYREPLAY_H
