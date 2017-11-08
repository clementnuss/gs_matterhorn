#include <memory>
#include <cassert>
#include "Factories.h"
#include "Utilities/ParsingUtilities.h"
#include "DatagramSpec.h"

/**
 * Builds a Telemetry struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return A Telemetry struct
 */
shared_ptr<IDeserializable> Factories::telemetryReadingFactory(std::vector<uint8_t> payloadBuffer) {
    assert(payloadBuffer.size() == PAYLOAD_TYPES_LENGTH.at(DatagramPayloadType::TELEMETRY));

    auto it = payloadBuffer.begin();

    auto measurement_time = parse32<uint32_t>(it);

    float ax = parse16<int16_t>(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;
    float ay = parse16<int16_t>(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;
    float az = parse16<int16_t>(it) * SensorConstants::MPU_ACCEL_MULTIPLIER;

    float gx = parse16<int16_t>(it);
    float gy = parse16<int16_t>(it);
    float gz = parse16<int16_t>(it);

    float mx = parse16<int16_t>(it);
    float my = parse16<int16_t>(it);
    float mz = parse16<int16_t>(it);

    float_cast temperature = {.uint32 = parse32<uint32_t>(it)};
    float_cast pressure = {.uint32 = parse32<uint32_t>(it)};
    float pressure_hPa = pressure.fl / 100.0f;

    double adjustedSealevelPressure = SensorConstants::currentLocationReferenceHPa * pow(
            (1 - (0.0065 * SensorConstants::currentLocationHeight) /
                 (SensorConstants::currentLocationTemperature + 0.006 * SensorConstants::currentLocationHeight +
                  273.15)), -5.257);

    double altitude = 44330 * (1.0 - pow(pressure_hPa / adjustedSealevelPressure, 0.1903));

    TelemetryReading r{measurement_time / 1000,
                       altitude,
                       {ax, ay, az},
                       {mx, my, mz},
                       {gx, gy, gz},
                       pressure_hPa,
                       temperature.fl};
    return std::make_shared<TelemetryReading>(r);
}
