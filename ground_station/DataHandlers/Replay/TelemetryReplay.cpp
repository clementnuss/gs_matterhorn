#include "TelemetryReplay.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <Utilities/TimeUtils.h>

using namespace boost::filesystem;

/**
 * Constructor for the TelemetryReplay class
 * @param path location of the file/directory to read.
 */
TelemetryReplay::TelemetryReplay(const std::string &path) :
        path_{path}, sensorsReadings_{}, lastPlaybackTime_{}, endReadingsIter_{}, frontReadingsIter_{},
        playbackSpeed_{1},
        playbackReversed_{}, lastTimeStamp_{0} {

    std::locale(std::cout.getloc(), new std::numpunct<char>{'.'});
}

void
TelemetryReplay::startup() {
    if (exists(path_)) {
        if (is_directory(path_.parent_path())) {
            path_ = path_.parent_path();
        }
        std::vector<path> directoryEntries;
        copy(directory_iterator(path_), directory_iterator(), std::back_inserter(directoryEntries));
        sort(directoryEntries.begin(), directoryEntries.end());

        for (path &f: directoryEntries) {
            if (is_regular_file(f)) {
                parseFile(f);
            }
        }
    }

    if (sensorsReadings_.empty()) {
        //Add a blank reading to prevent the UI from crashing
        SensorsPacket r{};
        sensorsReadings_.push_back(r);
        resetPlayback();
        throw (std::runtime_error("No reading has been found in the path: \n" + path_.string()));
    }

    if (gpsReadings_.empty()) {
        GPSPacket g{};
        gpsReadings_.push_back(g);
        std::cout << "No GPS reading for this replay" << endl;
    }

    resetPlayback();
}

std::vector<EventPacket>
TelemetryReplay::pollEventsData() {
    return std::vector<EventPacket>();
}

std::vector<GPSPacket>
TelemetryReplay::pollGPSData() {
    std::vector<GPSPacket> vec{};

    if (!playbackReversed_) {
        while ((*gpsEndReadingsIter_).timestamp_ < lastTimeStamp_) {
            if (gpsEndReadingsIter_ == gpsReadings_.end()) {
                break;
            }
            vec.push_back(*gpsEndReadingsIter_++);
        }
    }

    return vec;
}

std::vector<SensorsPacket>
TelemetryReplay::pollSensorsData() {
    std::vector<SensorsPacket> vec{};
    auto localLastPlaybackTime = std::chrono::system_clock::now();
    double adjustedTime = usecsBetween(lastPlaybackTime_, localLastPlaybackTime);
    adjustedTime *= playbackSpeed_;

    if (playbackReversed_) {

        double adjustedFrontTime =
                lastTimeStamp_ -
                UIConstants::GRAPH_MEMORY_USECS - adjustedTime;
        auto newFrontPosition = frontReadingsIter_;
        while ((*newFrontPosition).timestamp_ > adjustedFrontTime) {
            if (newFrontPosition == sensorsReadings_.begin()) {
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
        while (newEndReadingsPosition != sensorsReadings_.begin() &&
               (*newEndReadingsPosition).timestamp_ > adjustedLastTime) {
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
            if (endReadingsIter_ == sensorsReadings_.end()) {
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
                /*if (frontReadingsIter_ == sensorsReadings_.end()){
                    break;
                }*/
                frontReadingsIter_++;
            }
        }
    }

    return vec;
}

void
TelemetryReplay::parseFile(boost::filesystem::path p) {
    boost::filesystem::ifstream ifs{p, std::ios::in};
    std::cout << "Parsing telemetry file " << p.string() << endl;

    std::string reading;
    while (getline(ifs, reading)) {
        std::vector<std::string> values;
        boost::split(values, reading, boost::is_any_of("\t "),
                     boost::algorithm::token_compress_mode_type::token_compress_on);

        if (values.size() == 15) {
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
                        timestamp, seqNumber, FlyableType::ROCKET,
                        altitude, Data3D{ax, ay, az}, Data3D{mx, my, mz}, Data3D{gx, gy, gz},
                        pressure, temperature, air_speed};
                sensorsReadings_.push_back(std::move(r));

            } catch (std::logic_error &e) {
                std::cout << "\tunable to decode this reading:\n\t" << reading;
            }
        } else if (values.size() == 7) {
            try {
                auto it = values.begin();
                it++; //uint32_t seqNumber = std::stoul(*); // Invalid
                uint32_t timestamp = std::stoul(*it++);
                auto satsCount = static_cast<uint8_t>(std::stoul(*it++));
                auto hdop = static_cast<float>(std::stod(*it++));
                auto latitude = static_cast<float>(std::stod(*it++));
                auto longitude = static_cast<float>(std::stod(*it++));
                auto altitude = static_cast<float>(std::stod(*it++));

                GPSPacket g{timestamp, 0, FlyableType::ROCKET, satsCount, hdop, latitude, longitude, altitude};
                gpsReadings_.push_back(std::move(g));

            } catch (std::logic_error &e) {
                std::cout << "\tunable to decode this reading:\n\t" << reading;
            }
        } else {
            std::cout << "\tInvalid reading, only " << values.size() << " values on the line:" << endl;
            std::cout << "\t" << reading << endl;
            continue;
        }
    }
    ifs.close();
}

void
TelemetryReplay::updatePlaybackSpeed(double newPlaybackSpeed) {
    playbackSpeed_ = newPlaybackSpeed;
}

void
TelemetryReplay::resetPlayback() {
    playbackSpeed_ = 1;
    setPlaybackReversed(false);
    frontReadingsIter_ = sensorsReadings_.begin();
    endReadingsIter_ = sensorsReadings_.begin();
    gpsEndReadingsIter_ = gpsReadings_.begin();
    lastPlaybackTime_ = std::chrono::system_clock::now();
    lastTimeStamp_ = sensorsReadings_.front().timestamp_;
}

bool
TelemetryReplay::endOfPlayback() {
    if (playbackReversed_) {
        return endReadingsIter_ == sensorsReadings_.begin();
    } else {
        return endReadingsIter_ == sensorsReadings_.end();
    }
}

void
TelemetryReplay::setPlaybackReversed(bool reversed) {
    playbackReversed_ = reversed;
}

bool
TelemetryReplay::isReplayHandler() {
    return true;
}


