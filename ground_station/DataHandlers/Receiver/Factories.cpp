#include <memory>
#include "Factories.h"

shared_ptr<IDeserializable> Factories::telemetryReadingFactory(std::vector<uint8_t> payloadBuffer) {
    auto it = payloadBuffer.begin();

    long measurement_time = static_cast<long>(parseUint32(it));

    uint16_t ax = parseUint16(it);
    uint16_t ay = parseUint16(it);
    uint16_t az = parseUint16(it);

    uint16_t mx = parseUint16(it);
    uint16_t my = parseUint16(it);
    uint16_t mz = parseUint16(it);

    uint16_t gx = parseUint16(it);
    uint16_t gy = parseUint16(it);
    uint16_t gz = parseUint16(it);

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