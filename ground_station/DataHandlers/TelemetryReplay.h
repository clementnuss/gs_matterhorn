
#ifndef GS_MATTERHORN_TELEMETRYREPLAY_H
#define GS_MATTERHORN_TELEMETRYREPLAY_H

#include <boost/filesystem.hpp>
#include "TelemetryHandler.h"

class TelemetryReplay : public TelemetryHandler {

public:

    explicit TelemetryReplay(std::string &);

    void startup() override;

    vector<RocketEvent> pollEvents() override;

    vector<TelemetryReading> pollData() override;

private:
    boost::filesystem::path path_;
    vector<TelemetryReading> readings_;
    vector<TelemetryReading>::const_iterator lastReadingIter_;

    void parseFile(boost::filesystem::path);

    static TimedData parseLine(string &reading);
};

#endif //GS_MATTERHORN_TELEMETRYREPLAY_H
