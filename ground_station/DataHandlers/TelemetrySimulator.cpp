#include <DataStructures/datastructs.h>
#include <c++/vector>
#include <QtCore/QTime>
#include "TelemetrySimulator.h"

using namespace std;

//TODO: make telemetry handler factory to provide either simulator or real one

TelemetrySimulator::TelemetrySimulator() {
}

vector<TelemetryReading> TelemetrySimulator::getData() {

    return generateTelemetryVector();
}

const vector<TelemetryReading> TelemetrySimulator::generateTelemetryVector() {
    size_t vlength = static_cast<size_t>(
            qrand() / static_cast<double>(RAND_MAX) * SimulatorConstants::MAX_RANDOM_VECTOR_LENGTH + 1);
    vector<TelemetryReading> v;

    for (size_t i = 0; i < vlength; i++) {
        v.push_back(generateTelemetry());
    }

    return v;
}

const TelemetryReading TelemetrySimulator::generateTelemetry() {

    static QTime time{QTime::currentTime()};
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));

    long key = time.elapsed();
    double keysec = key / 1000.0;

    double rnd = qrand();

    return TelemetryReading{
            key,
            DataReading{10000 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 0.2 * sin(keysec / 0.8), true},
            DataReading{900 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 0.2 * sin(keysec / 0.38), true},
            DataReading{200 * sin(keysec) + rnd / static_cast<double>(RAND_MAX) * 0.3 * sin(keysec / 0.27), true},
            DataReading{100 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 0.2 * sin(keysec / 0.6), true},
            DataReading{50 * (keysec) + rnd / static_cast<double>(RAND_MAX) * 0.2 * sin(keysec / 0.7), true},
            YawPitchRollReading{
                    sin(key) + rnd / static_cast<double>(RAND_MAX) * 1 * sin(keysec / 0.7),
                    sin(key) + rnd / static_cast<double>(RAND_MAX) * 1 * sin(keysec / 0.8),
                    sin(key) + rnd / static_cast<double>(RAND_MAX) * 1 * sin(keysec / 0.9),
                    true}
    };
}
