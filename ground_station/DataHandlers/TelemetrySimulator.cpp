#include <DataStructures/datastructs.h>
#include <vector>
#include <QtCore/QTime>
#include <cassert>
#include <iostream>
#include <cmath>
#include "TelemetrySimulator.h"

using namespace std;
using namespace SimulatorConstants;

//TODO: make telemetry handler factory to provide either simulator or real one

TelemetrySimulator::TelemetrySimulator() : time{QTime::currentTime()} {
}

vector<TelemetryReading> TelemetrySimulator::getData() {
    return generateTelemetryVector();
}

vector<RocketEvent> TelemetrySimulator::getEvents() {
    vector<RocketEvent> v;

    if ((qrand() / static_cast<double>(RAND_MAX)) * EVENT_PROBABILITY_INTERVAL <= 1) {
        RocketEvent r = generateEvent();
        cout << "Generating event: " << r.description << endl;
        v.push_back(r);
    }

    return v;
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
            DataReading{10000 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 1000.0 * sin(keysec / 0.8), true},
            DataReading{900 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 90.0 * sin(keysec / 0.38), true},
            DataReading{200 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 20.0 * sin(keysec / 0.27), true},
            DataReading{100 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 10.0 * sin(keysec / 0.6), true},
            DataReading{50 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 5.0 * sin(keysec / 0.7), true},
            YawPitchRollReading{
                    90.0 * sin(key) + rnd / static_cast<double>(RAND_MAX) * 1 * sin(keysec / 0.7),
                    90.0 * sin(key) + rnd / static_cast<double>(RAND_MAX) * 1 * sin(keysec / 0.8),
                    90.0 * sin(key) + rnd / static_cast<double>(RAND_MAX) * 1 * sin(keysec / 0.9),
                    true}
    };
}

RocketEvent TelemetrySimulator::generateEvent() {

    // Select an event randomly
    int code = static_cast<int>(round((EVENT_CODES.size() - 1) * qrand() / static_cast<double>(RAND_MAX)));

    assert(EVENT_CODES.find(code) != EVENT_CODES.end());
    return RocketEvent {time.elapsed(), code, EVENT_CODES.at(code)};
}
