#ifndef GS_MATTERHORN_GRAPHUTILS_H
#define GS_MATTERHORN_GRAPHUTILS_H


QCPGraphData accelerationFromReading(TelemetryReading reading) {
    return {reading.timestamp_ / 1000.0, reading.acceleration_.norm()};
}


QCPGraphData altitudeFromReading(TelemetryReading reading) {
    return {reading.timestamp_ / 1000.0, reading.altitude_};
}


QCPGraphData speedFromReading(TelemetryReading reading) {
//    return {static_cast<double>(reading.timestamp), reading.speed.value};
    return {reading.timestamp_ / 1000.0, -1};
}

#endif //GS_MATTERHORN_GRAPHUTILS_H
