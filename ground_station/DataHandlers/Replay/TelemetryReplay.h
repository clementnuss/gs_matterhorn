
#ifndef GS_MATTERHORN_TELEMETRYREPLAY_H
#define GS_MATTERHORN_TELEMETRYREPLAY_H

#include <boost/filesystem.hpp>
#include <chrono>
#include "DataHandlers/IReceiver.h"
#include "ITelemetryReplayHandler.h"
#include "DataStructures/Datastructs.h"

class TelemetryReplay : public IReceiver, public ITelemetryReplayHandler {

public:

    explicit TelemetryReplay(const std::string &);

    void startup() override;

    std::vector<EventPacket> pollEventsData();

    std::vector<SensorsPacket> pollSensorsData();

    std::vector<GPSPacket> pollGPSData();

    void updatePlaybackSpeed(double) override;

    void setPlaybackReversed(bool) override;

    void resetPlayback() override;

    bool endOfPlayback() override;

    bool isReplayHandler();

    void sendCommand(const uint8_t *, size_t) {

    }


private:
    boost::filesystem::path path_;
    std::vector<GPSPacket> gpsReadings_;
    std::vector<GPSPacket>::const_iterator gpsEndReadingsIter_;

    std::vector<SensorsPacket> sensorsReadings_;
    std::vector<SensorsPacket>::const_iterator endReadingsIter_;
    std::vector<SensorsPacket>::const_iterator frontReadingsIter_;
    std::chrono::system_clock::time_point lastPlaybackTime_;
    uint32_t lastTimeStamp_;
    double playbackSpeed_;
    bool playbackReversed_;

    void parseFile(boost::filesystem::path);

};

#endif //GS_MATTERHORN_TELEMETRYREPLAY_H
