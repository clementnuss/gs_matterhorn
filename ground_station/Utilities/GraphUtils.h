#ifndef GS_MATTERHORN_GRAPHUTILS_H
#define GS_MATTERHORN_GRAPHUTILS_H

QCPGraphData speedFromReading(TelemetryReading reading) {
//    return {static_cast<double>(reading.timestamp), reading.speed.value};
    return {static_cast<double>(reading.timestamp), -1};
}

QCPGraphData accelerationFromReading(TelemetryReading reading) {
    return {static_cast<double>(reading.timestamp), reading.acceleration.norm()};
}

#endif //GS_MATTERHORN_GRAPHUTILS_H
