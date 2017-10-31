#include <DataStructures/datastructs.h>
#include <vector>
#include <QtCore/QTime>
#include <cassert>
#include <iostream>
#include <c++/chrono>
#include <Utilities/TimeUtils.h>
#include "TelemetrySimulator.h"

using namespace std;
using namespace SimulatorConstants;

//TODO: make telemetry handler factory to provide either simulator or real one

TelemetrySimulator::TelemetrySimulator() : time{QTime::currentTime()},
                                           timeOfLastPolledData{chrono::system_clock::now()}, variableRate{true} {
}

vector<TelemetryReading> TelemetrySimulator::pollData() {

    vector<TelemetryReading> generatedVector = generateTelemetryVector();
    chrono::system_clock::time_point now = chrono::system_clock::now();

    if (variableRate) {
        updateHandlerStatus();
        switch (simulatorStatus) {
            case HandlerStatus::NOMINAL:
                timeOfLastPolledData = now;
                return generatedVector;

            case HandlerStatus::LOSSY: {
                long long millisSinceLastPoll = msecsBetween(timeOfLastPolledData, now);
                cout << "Simu: " << millisSinceLastPoll << endl << flush;
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

    return vector<TelemetryReading>();
}

vector<RocketEvent> TelemetrySimulator::pollEvents() {
    vector<RocketEvent> v;

    if ((qrand() / static_cast<double>(RAND_MAX)) * EVENT_PROBABILITY_INTERVAL <= 1) {
        RocketEvent r = generateEvent();
        cout << "Generating event: " << r.description << endl;
        v.push_back(r);
    }

    return v;
}

void TelemetrySimulator::setVariableRate(bool enable) {
    variableRate = enable;
}

void TelemetrySimulator::startup() {

}

const vector<TelemetryReading> TelemetrySimulator::generateTelemetryVector() {
    auto vlength = static_cast<size_t>(
            qrand() / static_cast<double>(RAND_MAX) * MAX_RANDOM_VECTOR_LENGTH + 1);
    vector<TelemetryReading> v;

    for (size_t i = 0; i < vlength; i++) {
        v.push_back(generateTelemetry());
    }

    return v;
}

const TelemetryReading TelemetrySimulator::generateTelemetry() {

    long key = time.elapsed();
    double keysec = key / 1000.0;

    double rnd = qrand();

    return TelemetryReading{
            key,
            10000 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 1000.0 * sin(keysec / 0.8),
            XYZReading{900 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 90.0 * sin(keysec / 0.38), 0, 0},
            XYZReading{200 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 20.0 * sin(keysec / 0.27), 0, 0},
            XYZReading{100 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 10.0 * sin(keysec / 0.6), 1, 2},
            50 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 5.0 * sin(keysec / 0.7),
            90.0 * sin(key) + rnd / static_cast<double>(RAND_MAX) * 1 * sin(keysec / 0.7)
    };
}

RocketEvent TelemetrySimulator::generateEvent() {

    // Select an event randomly
    int code = static_cast<int>(round((EVENT_CODES.size() - 1) * qrand() / static_cast<double>(RAND_MAX)));

    assert(EVENT_CODES.find(code) != EVENT_CODES.end());
    return RocketEvent {time.elapsed(), code, EVENT_CODES.at(code)};
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