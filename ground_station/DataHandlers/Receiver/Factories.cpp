#include <memory>
#include "Factories.h"
#include "Utilities/ParsingUtilities.h"

shared_ptr<IDeserializable> Factories::telemetryReadingFactory(std::vector<uint8_t> payloadBuffer) {
    auto it = payloadBuffer.begin();

    auto measurement_time = parseUint32(it) / 1000;

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
