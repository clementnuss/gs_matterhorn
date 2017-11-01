#include <memory>
#include "Factories.h"
#include "Utilities/ParsingUtilities.h"

shared_ptr<IDeserializable> Factories::telemetryReadingFactory(std::vector<uint8_t> payloadBuffer) {
    auto it = payloadBuffer.begin();

    auto measurement_time = parse32<uint32_t>(it) / 1000;

    float ax = parse16<int16_t>(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;
    float ay = parse16<int16_t>(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;
    float az = parse16<int16_t>(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;

    float mx = parse16<int16_t>(it);
    float my = parse16<int16_t>(it);
    float mz = parse16<int16_t>(it);

    float gx = parse16<int16_t>(it);
    float gy = parse16<int16_t>(it);
    float gz = parse16<int16_t>(it);

    float_cast temperature = {.uint32 = parse32<uint32_t>(it)};
    float_cast pressure = {.uint32 = parse32<uint32_t>(it)};
    float_cast altitude = {.uint32 = parse32<uint32_t>(it)};

    TelemetryReading r{measurement_time,
                       altitude.fl,
                       {ax, ay, az},
                       {mx, my, mz},
                       {gx, gy, gz},
                       pressure.fl,
                       temperature.fl};
    return std::make_shared<TelemetryReading>(r);
}
