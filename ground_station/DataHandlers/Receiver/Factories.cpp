#include <memory>
#include "Factories.h"

shared_ptr<IDeserializable> Factories::telemetryReadingFactory(std::vector<uint8_t> payloadBuffer) {
    auto it = payloadBuffer.begin();

    long measurement_time = static_cast<long>(parseUint32(it)) / 1000;

    float ax = parseInt16(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;
    float ay = parseInt16(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;
    float az = parseInt16(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;

    float mx = parseInt16(it);
    float my = parseInt16(it);
    float mz = parseInt16(it);

    float gx = parseInt16(it);
    float gy = parseInt16(it);
    float gz = parseInt16(it);

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


int16_t Factories::parseInt16(vector<uint8_t>::iterator &it) {
    return (*(it++) << 8) | *(it++);
}

uint32_t Factories::parseUint32(vector<uint8_t>::iterator &it) {
    return (*(it++) << 24) | (*(it++) << 16) | (*(it++) << 8) | *(it++);
}