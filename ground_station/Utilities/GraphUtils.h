#ifndef GS_MATTERHORN_GRAPHUTILS_H
#define GS_MATTERHORN_GRAPHUTILS_H


QCPGraphData accelerationFromReading(SensorsPacket reading) {
    return {reading.timestamp_ / 1'000'000.0, reading.acceleration_.norm()};
}


QCPGraphData altitudeFromReading(SensorsPacket reading) {
    return {reading.timestamp_ / 1'000'000.0, reading.altitude_};
}


QCPGraphData speedFromReading(SensorsPacket reading) {
//    return {static_cast<double>(reading.timestamp), reading.speed.value};
    return {reading.timestamp_ / 1'000'000.0, -1};
}

#endif //GS_MATTERHORN_GRAPHUTILS_H
