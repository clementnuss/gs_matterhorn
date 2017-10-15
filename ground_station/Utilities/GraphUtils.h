#ifndef GS_MATTERHORN_GRAPHUTILS_H
#define GS_MATTERHORN_GRAPHUTILS_H

QCPGraphData speedFromReading(TelemetryReading reading) {
    return {reading.timestamp, reading.speed.value};
}

QCPGraphData accelerationFromReading(TelemetryReading reading) {
    return {reading.timestamp, reading.acceleration.value};
}

#endif //GS_MATTERHORN_GRAPHUTILS_H
