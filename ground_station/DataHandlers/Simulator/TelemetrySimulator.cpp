#include <DataStructures/datastructs.h>
#include <cassert>
#include <iostream>
#include <Utilities/TimeUtils.h>
#include <QtGui/QtGui>
#include "TelemetrySimulator.h"

using namespace std;
using namespace SimulatorConstants;

//TODO: make telemetry handler factory to provide either simulator or real one

TelemetrySimulator::TelemetrySimulator() : timeOfLastPolledData{chrono::system_clock::now()},
                                           timeOfLastPolledGeoData{chrono::system_clock::now()},
                                           timeOfInitialization{chrono::system_clock::now()},
                                           variableRate{true},
                                           geoDataTriggered_{false},
                                           sequenceNumber_{0},
                                           simulatorStatus{HandlerStatus::NOMINAL} {
}

vector<SensorsPacket> TelemetrySimulator::pollData() {

    vector<SensorsPacket> generatedVector = generateTelemetryVector();
    chrono::system_clock::time_point now = chrono::system_clock::now();

    if (variableRate) {
        updateHandlerStatus();
        switch (simulatorStatus) {
            case HandlerStatus::NOMINAL:
                timeOfLastPolledData = now;
                return generatedVector;

            case HandlerStatus::LOSSY: {
                long long millisSinceLastPoll = msecsBetween(timeOfLastPolledData, now);
                if (millisSinceLastPoll > CommunicationsConstants::MSECS_NOMINAL_RATE) {
                    timeOfLastPolledData = now;
                    return generatedVector;
                }
                break;
            }

            case HandlerStatus::DOWN:
                break;

            default:
                break;
        }

    } else {
        return generateTelemetryVector();
    }

    return vector<SensorsPacket>();
}

vector<EventPacket> TelemetrySimulator::pollEvents() {
    vector<EventPacket> v;

    if ((qrand() / static_cast<double>(RAND_MAX)) * EVENT_PROBABILITY_INTERVAL <= 1) {
        EventPacket r = generateEvent();
        cout << "Generating event: " << r.description << endl;
        v.push_back(r);
    }

    return v;
}

vector<Data3D> TelemetrySimulator::pollLocations() {

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    vector<Data3D> generatedVector{};

    long long millisSinceLastPoll = msecsBetween(timeOfLastPolledGeoData, now);
    if (millisSinceLastPoll > TimeConstants::MSECS_IN_SEC / 10) {
        timeOfLastPolledGeoData = now;

        long long int msecs = msecsBetween(timeOfInitialization, chrono::system_clock::now());

        double x = 2 * static_cast<double>(msecs) / 10;

        Data3D r;

        r.x_ = 0;
        r.y_ = 50 * sqrt(x);
        r.z_ = -x;

        generatedVector.push_back(r);
    }

    return generatedVector;
}

void TelemetrySimulator::setVariableRate(bool enable) {
    variableRate = enable;
}

void TelemetrySimulator::startup() {

}

const vector<SensorsPacket> TelemetrySimulator::generateTelemetryVector() {
    auto vlength = static_cast<size_t>(
            qrand() / static_cast<double>(RAND_MAX) * MAX_RANDOM_VECTOR_LENGTH + 1);
    vector<SensorsPacket> v;

    for (size_t i = 0; i < vlength; i++) {
        v.push_back(generateTelemetry());
    }

    return v;
}

const SensorsPacket TelemetrySimulator::generateTelemetry() {

    long long int msecs = msecsBetween(timeOfInitialization, chrono::system_clock::now());
    double keysec = msecs / 1000.0;

    double rnd = qrand();
    return SensorsPacket{
            static_cast<uint32_t>(msecs),
            10000 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 1000.0 * sin(keysec / 0.8),
            Data3D{
                    900 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 90.0 * sin(keysec / 0.38),
                    900 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 90.0 * sin(keysec / 0.37),
                    900 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 90.0 * sin(keysec / 0.36)
            },
            Data3D{
                    200 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 20.0 * sin(keysec / 0.27),
                    200 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 20.0 * sin(keysec / 0.26),
                    200 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 20.0 * sin(keysec / 0.25)
            },
            Data3D{
                    100 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 10.0 * sin(keysec / 0.6),
                    100 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 10.0 * sin(keysec / 0.5),
                    100 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 10.0 * sin(keysec / 0.4)
            },
            50 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 5.0 * sin(keysec / 0.7),
            90.0 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 1 * sin(keysec / 0.7),
            14 + 30 * sin(keysec),
            sequenceNumber_++
    };
}

EventPacket TelemetrySimulator::generateEvent() {

    // Select an event randomly
    auto code = static_cast<int>(round(
            (RocketEventConstants::EVENT_CODES.size() - 1) * qrand() / static_cast<double>(RAND_MAX)));
    long long int msecs = msecsBetween(timeOfInitialization, chrono::system_clock::now());

    assert(RocketEventConstants::EVENT_CODES.find(code) != RocketEventConstants::EVENT_CODES.end());
    return EventPacket {static_cast<uint32_t>(msecs), code, RocketEventConstants::EVENT_CODES.at(code)};
}


void TelemetrySimulator::updateHandlerStatus() {
    chrono::system_clock::time_point now = chrono::system_clock::now();
    long long seconds =
            chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count() / TimeConstants::MSECS_IN_SEC;
    double indicator = cos(seconds * VARIABLE_RATE_TIME_MULTIPLIER);

    if (indicator <= 0) {
        simulatorStatus = HandlerStatus::NOMINAL;
    } else if (0 < indicator && indicator < 0.9) {
        simulatorStatus = HandlerStatus::LOSSY;
    } else {
        simulatorStatus = HandlerStatus::DOWN;
    }
}

bool TelemetrySimulator::isReplayHandler() {
    return false;
}
