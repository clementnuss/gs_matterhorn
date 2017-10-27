#include <memory>
#include "Factories.h"

shared_ptr<IDeserializable> Factories::telemetryReadingFactory(std::vector<uint8_t> payloadBuffer) {
    auto it = payloadBuffer.begin();

    long measurement_time = static_cast<long>(parseUint32(it));

    float ax = parseUint16(it);
    float ay = parseUint16(it);
    float az = parseUint16(it);

    float mx = parseUint16(it);
    float my = parseUint16(it);
    float mz = parseUint16(it);

    float gx = parseUint16(it);
    float gy = parseUint16(it);
    float gz = parseUint16(it);

    float_cast temperature = {.uint32 = parseUint32(it)};
    float_cast pressure = {.uint32 = parseUint32(it)};
    float_cast altitude = {.uint32 = parseUint32(it)};

    TelemetryReading r{measurement_time,
                       altitude.fl,
                       {ax, ay, az},
                       {mx, my, mz},
                       {gx, gy, gz},
                       pressure.fl,
                       temperature.fl};
    return std::make_shared<TelemetryReading>(r);
}


uint16_t Factories::parseUint16(vector<uint8_t>::iterator &it) {
    return (*(it++) << 8) | *(it++);
}

uint32_t Factories::parseUint32(vector<uint8_t>::iterator &it) {
    return (*(it++) << 24) | (*(it++) << 16) | (*(it++) << 8) | *(it++);
}