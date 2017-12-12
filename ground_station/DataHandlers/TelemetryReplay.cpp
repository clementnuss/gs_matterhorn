#include "TelemetryReplay.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <Utilities/TimeUtils.h>

using namespace boost::filesystem;

/**
 * Constructor for the TelemetryReplay class
 * @param path location of the file/directory to read.
 */
TelemetryReplay::TelemetryReplay(std::string &path) :
        path_{path}, readings_{}, lastPlaybackTime_{}, endReadingsIter_{}, frontReadingsIter_{}, playbackSpeed_{1},
        playbackReversed_{}, lastTimeStamp_{0} {}

void TelemetryReplay::startup() {
    if (exists(path_)) {
        if (is_regular_file((path_))) {
            parseFile(path_);
        } else if (is_directory((path_))) {
            vector<path> directoryEntries;
            copy(directory_iterator(path_), directory_iterator(), std::back_inserter(directoryEntries));
            sort(directoryEntries.begin(), directoryEntries.end());

            for (path &f: directoryEntries) {
                if (is_regular_file(f)) {
                    parseFile(f);
                }
            }
        } else {
            throw (std::runtime_error(path_.string() + " exists, but is not a regular file or directory\n"));
        }
    } else {
        throw (std::runtime_error(path_.string() + " does not exist\n"));
    }

    resetPlayback();
}

vector<RocketEvent> TelemetryReplay::pollEvents() {
    return vector<RocketEvent>();
}

vector<TelemetryReading> TelemetryReplay::pollData() {
    vector<TelemetryReading> vec{};
    double adjustedTime = usecsBetween(lastPlaybackTime_, std::chrono::system_clock::now());

    adjustedTime *= playbackSpeed_;

    if (playbackReversed_) {

        double adjustedFrontTime =
                lastTimeStamp_ -
                UIConstants::GRAPH_MEMORY_USECS - adjustedTime;
        auto newFrontPosition = frontReadingsIter_;
        while ((*newFrontPosition).timestamp_ > adjustedFrontTime) {
            if (newFrontPosition == readings_.begin()) {
                continue;
            }
            newFrontPosition--;
        }
        if (newFrontPosition != frontReadingsIter_) {
            vec.insert(vec.begin(), newFrontPosition, --frontReadingsIter_);
            frontReadingsIter_ = newFrontPosition;
        }

        double adjustedLastTime = (double) lastTimeStamp_ - adjustedTime;
        auto newEndReadingsPosition = endReadingsIter_;
        while (newEndReadingsPosition != readings_.begin() && (*newEndReadingsPosition).timestamp_ > adjustedLastTime) {
            newEndReadingsPosition--;
        }

        if (newEndReadingsPosition != endReadingsIter_) {
            lastTimeStamp_ = (*newEndReadingsPosition).timestamp_;
            endReadingsIter_ = newEndReadingsPosition;
            lastPlaybackTime_ = std::chrono::system_clock::now();
        }

    } else {
        adjustedTime += lastTimeStamp_;
        while ((*endReadingsIter_).timestamp_ < adjustedTime) {
            if (endReadingsIter_ == readings_.end()) {
                continue;
            }
            vec.push_back(*endReadingsIter_++);
            lastTimeStamp_ = (*endReadingsIter_).timestamp_;
        }

        if (!vec.empty()) {
            lastTimeStamp_ = (*--vec.end()).timestamp_;
            lastPlaybackTime_ = std::chrono::system_clock::now();
        }

        if ((*endReadingsIter_).timestamp_ >= UIConstants::GRAPH_MEMORY_USECS) /* Test that we don't have overflow*/{
            while (((*endReadingsIter_).timestamp_ - UIConstants::GRAPH_MEMORY_USECS) >
                   (*frontReadingsIter_).timestamp_) {
                /*if (frontReadingsIter_ == readings_.end()){
                    break;
                }*/
                frontReadingsIter_++;
            }
        }
    }

    return vec;
}

void TelemetryReplay::parseFile(boost::filesystem::path p) {
    boost::filesystem::ifstream ifs{p, ios::in};
    cout << "Parsing telemetry file " << p.string() << endl;

    string reading;
    while (getline(ifs, reading)) {
        std::vector<std::string> values;
        boost::split(values, reading, boost::is_any_of("\t ,"),
                     boost::algorithm::token_compress_mode_type::token_compress_on);

        if (values.size() != 15) {
            cout << "\tInvalid reading, only " << values.size() << " values on the line:" << endl;
            cout << "\t" << reading << endl;
        }
        try {

            auto it = values.begin();
            uint32_t seqNumber = std::stoul(*it++);
            uint32_t timestamp = std::stoul(*it++);
            double altitude = std::stod(*it++);
            double air_speed = std::stod(*it++);
            double pressure = std::stod(*it++);
            double temperature = std::stod(*it++);
            double ax = std::stod(*it++);
            double ay = std::stod(*it++);
            double az = std::stod(*it++);
            double gx = std::stod(*it++);
            double gy = std::stod(*it++);
            double gz = std::stod(*it++);
            double mx = std::stod(*it++);
            double my = std::stod(*it++);
            double mz = std::stod(*it++);

            TelemetryReading r{
                    timestamp, altitude, XYZReading{ax, ay, az}, XYZReading{mx, my, mz}, XYZReading{gx, gy, gz},
                    pressure, temperature, air_speed, seqNumber};
            readings_.push_back(std::move(r));

        } catch (std::logic_error &e) {
            cout << "\tunable to decode this reading:\n\t" << reading;
        }
    }
    ifs.close();
}

void TelemetryReplay::updatePlaybackSpeed(double newPlaybackSpeed) {
    playbackSpeed_ = newPlaybackSpeed;
}

void TelemetryReplay::resetPlayback() {
    playbackSpeed_ = 1;
    setPlaybackReversed(false);
    frontReadingsIter_ = readings_.begin();
    endReadingsIter_ = readings_.begin();
    lastPlaybackTime_ = std::chrono::system_clock::now();
    lastTimeStamp_ = readings_.front().timestamp_;
}

bool TelemetryReplay::endOfPlayback() {
    if (playbackReversed_) {
        return endReadingsIter_ == readings_.begin();
    } else {
        return endReadingsIter_ == readings_.end();
    }
}

void TelemetryReplay::setPlaybackReversed(bool reversed) {
    playbackReversed_ = reversed;
}

vector<XYZReading> TelemetryReplay::pollLocations() {
    return vector<XYZReading>();
}

