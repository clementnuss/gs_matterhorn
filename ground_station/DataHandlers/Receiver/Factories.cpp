#include <memory>
#include <cassert>
#include <Utilities/SensorUtils.h>
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
    assert(payloadBuffer.size() == PayloadType::TELEMETRY.length());

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

    // more info here: https://barani.biz/apps/sea-level-pressure/
    /*
    double adjustedSeaLevelPressure = SensorConstants::currentLocationReferenceHPa * pow(
            (1 - (0.0065 * SensorConstants::currentLocationHeight) /
                 (SensorConstants::currentLocationTemperature + 0.006 * SensorConstants::currentLocationHeight +
                  273.15)), -5.257);
      */

    //TODO: factorize functions and test independently
    double altitude = altitudeFromPressure(pressure_hPa);

    auto pitotPressure = parse16<uint16_t>(it);

    double air_speed = airSpeedFromPitotPressure(pitotPressure);

    SensorsPacket r{measurement_time,
                    altitude,
                    {ax, ay, az},
                    {mx, my, mz},
                    {gx, gy, gz},
                    pressure_hPa,
                    temperature.fl,
                    air_speed,
                    0};

    return std::make_shared<SensorsPacket>(r);
}

/**
 * Builds an Event struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return An Event struct
 */
shared_ptr<IDeserializable> Factories::telemetryEventFactory(std::vector<uint8_t> payloadBuffer) {
    assert(payloadBuffer.size() == PayloadType::EVENT.length());

    auto it = payloadBuffer.begin();

    auto measurement_time = parse32<uint32_t>(it);
    auto eventCode = parse8<uint8_t>(it);

    EventPacket r{};

    r.timestamp_ = measurement_time;

    if (RocketEventConstants::EVENT_CODES.find(eventCode) != RocketEventConstants::EVENT_CODES.end()) {
        r.code = eventCode;

    } else {
        r.code = RocketEventConstants::INVALID_EVENT_CODE;
    }

    return std::make_shared<EventPacket>(r);
}

/**
 * Builds a Control struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return A Control struct
 */
shared_ptr<IDeserializable> Factories::telemetryControlFactory(std::vector<uint8_t> payloadBuffer) {
    assert(payloadBuffer.size() == PayloadType::CONTROL.length());

    auto it = payloadBuffer.begin();
    auto measurement_time = parse32<uint32_t>(it);
    auto partCode = parse8<uint8_t>(it);
    auto statusValue = parse16<uint16_t>(it);

    ControlPacket r{};

    r.timestamp_ = measurement_time;

    if (ControlConstants::CONTROL_PARTS_CODES.find(partCode) != ControlConstants::CONTROL_PARTS_CODES.end()) {
        r.partCode_ = partCode;
        r.statusValue_ = statusValue;

    } else {
        r.partCode_ = ControlConstants::INVALID_PART_CODE;
        r.statusValue_ = ControlConstants::INVALID_STATUS_VALUE;
    }

    return std::make_shared<ControlPacket>(r);
}


/**
 * Builds a GPS struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return A GPS struct
 */
shared_ptr<IDeserializable> Factories::telemetryGPSFactory(std::vector<uint8_t> payloadBuffer) {
    assert(payloadBuffer.size() == PayloadType::GPS.length());

    auto it = payloadBuffer.begin();

    auto measurement_time = parse32<uint32_t>(it);

    uint8_t satsCount = parse8<uint8_t>(it);

    float_cast hdop = {.uint32 = parse32<uint32_t>(it)};
    float_cast lat = {.uint32 = parse32<uint32_t>(it)};
    float_cast lon = {.uint32 = parse32<uint32_t>(it)};
    int32_t alt = parse32<int32_t>(it);

    GPSPacket g{measurement_time,
                satsCount,
                hdop.fl,
                lat.fl,
                lon.fl,
                alt / 100.0f};

    return std::make_shared<GPSPacket>(g);
}