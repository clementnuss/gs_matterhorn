#include "TelemetryReplay.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <Utilities/TimeUtils.h>
#include <QtWidgets/QFileDialog>

using namespace boost::filesystem;

/**
 * Constructor for the TelemetryReplay class
 * @param path location of the file/directory to read.
 */
TelemetryReplay::TelemetryReplay(const string &path) :
        path_{path}, readings_{}, lastPlaybackTime_{}, endReadingsIter_{}, frontReadingsIter_{}, playbackSpeed_{1},
        playbackReversed_{}, lastTimeStamp_{0} {}

void TelemetryReplay::startup() {
    if (exists(path_)) {
        if (is_directory(path_.parent_path())) {
            path_ = path_.parent_path();
        }
        vector<path> directoryEntries;
        copy(directory_iterator(path_), directory_iterator(), std::back_inserter(directoryEntries));
        sort(directoryEntries.begin(), directoryEntries.end());

        for (path &f: directoryEntries) {
            if (is_regular_file(f)) {
                parseFile(f);
            }
        }
    }

    if (readings_.empty()) {
        //Add a blank reading to prevent the UI from crashing
        SensorsPacket r{};
        readings_.push_back(r);
        resetPlayback();
        throw (std::runtime_error("No reading has been found in the path: \n" + path_.string()));
    }

    resetPlayback();
}

vector<EventPacket> TelemetryReplay::pollEvents() {
    return vector<EventPacket>();
}

vector<SensorsPacket> TelemetryReplay::pollData() {
    vector<SensorsPacket> vec{};
    auto localLastPlaybackTime = std::chrono::system_clock::now();
    double adjustedTime = usecsBetween(lastPlaybackTime_, localLastPlaybackTime);
    adjustedTime *= playbackSpeed_;

    if (playbackReversed_) {

        double adjustedFrontTime =
                lastTimeStamp_ -
                UIConstants::GRAPH_MEMORY_USECS - adjustedTime;
        auto newFrontPosition = frontReadingsIter_;
        while ((*newFrontPosition).timestamp_ > adjustedFrontTime) {
            if (newFrontPosition == readings_.begin()) {
                break;
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
            lastPlaybackTime_ = localLastPlaybackTime;
        }

    } else {
        adjustedTime += lastTimeStamp_;
        while ((*endReadingsIter_).timestamp_ < adjustedTime) {
            if (endReadingsIter_ == readings_.end()) {
                break;
            }
            vec.push_back(*endReadingsIter_++);
            lastTimeStamp_ = (*endReadingsIter_).timestamp_;
        }

        if (!vec.empty()) {
            lastTimeStamp_ = (*--vec.end()).timestamp_;
            lastPlaybackTime_ = localLastPlaybackTime;
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
            continue;
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

            SensorsPacket r{
                    timestamp, altitude, Data3D{ax, ay, az}, Data3D{mx, my, mz}, Data3D{gx, gy, gz},
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

bool TelemetryReplay::isReplayHandler() {
    return true;
}

vector<Data3D> TelemetryReplay::pollLocations() {
    return vector<Data3D>();
}

