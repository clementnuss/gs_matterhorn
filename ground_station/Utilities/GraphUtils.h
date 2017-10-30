#ifndef GS_MATTERHORN_GRAPHUTILS_H
#define GS_MATTERHORN_GRAPHUTILS_H


QCPGraphData accelerationFromReading(TelemetryReading reading) {
    return {static_cast<double>(reading.timestamp_), reading.acceleration_.norm()};
}


QCPGraphData altitudeFromReading(TelemetryReading reading) {
    return {static_cast<double>(reading.timestamp_), reading.altitude_};
}


QCPGraphData speedFromReading(TelemetryReading reading) {
//    return {static_cast<double>(reading.timestamp), reading.speed.value};
    return {static_cast<double>(reading.timestamp_), -1};
}

#endif //GS_MATTERHORN_GRAPHUTILS_H
