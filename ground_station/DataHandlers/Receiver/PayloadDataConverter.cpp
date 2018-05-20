#include <memory>
#include <cassert>
#include <Utilities/SensorUtils.h>
#include "PayloadDataConverter.h"
#include "Utilities/ParsingUtilities.h"

/**
 * Builds a Telemetry struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return A Telemetry struct
 */
SensorsPacket *
PayloadDataConverter::toERT18SensorsPacket(const FlyableType &flyableType, const uint32_t &sequenceNumber, const std::vector<uint8_t> &payloadBuffer) {
    assert(payloadBuffer.size() == PayloadType::TELEMETRY_ERT18.length());

    auto it = payloadBuffer.begin();

    auto measurement_time = 1000 * parse32<uint32_t>(it); //millis

    float_cast ax = {.uint32 = parse32<uint32_t>(it)};
    float_cast ay = {.uint32 = parse32<uint32_t>(it)};
    float_cast az = {.uint32 = parse32<uint32_t>(it)};

    float_cast eulerX = {.uint32 = parse32<uint32_t>(it)};
    float_cast eulerY = {.uint32 = parse32<uint32_t>(it)};
    float_cast eulerZ = {.uint32 = parse32<uint32_t>(it)};

    float_cast temperature = {.uint32 = parse32<uint32_t>(it)};
    float_cast pressure = {.uint32 = parse32<uint32_t>(it)};
    float pressure_hPa = pressure.fl;

    // more info here: https://barani.biz/apps/sea-level-pressure/
    /*
    double adjustedSeaLevelPressure = SensorConstants::currentLocationReferenceHPa * pow(
            (1 - (0.0065 * SensorConstants::currentLocationHeight) /
                 (SensorConstants::currentLocationTemperature + 0.006 * SensorConstants::currentLocationHeight +
                  273.15)), -5.257);
      */

    //TODO: factorize functions and test independently
    double altitude = altitudeFromPressure(pressure_hPa);

//    double air_speed = airSpeedFromPitotPressure(pitotPressure);

    return new SensorsPacket(measurement_time,
                             sequenceNumber,
                             flyableType,
                             altitude,
                             Data3D{ax.fl, ay.fl, az.fl},
                             Data3D{eulerX.fl, eulerY.fl, eulerZ.fl},
                             Data3D {0, 0, 0},
                             pressure_hPa,
                             temperature.fl,
                             0.0);
}

/**
 * Builds a Telemetry struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return A Telemetry struct
 */
SensorsPacket *
PayloadDataConverter::toSensorsPacket(const FlyableType &flyableType, const uint32_t &sequenceNumber, const std::vector<uint8_t> &payloadBuffer) {
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

    return new SensorsPacket(measurement_time,
                             sequenceNumber,
                             flyableType,
                             altitude,
                             Data3D{ax, ay, az},
                             Data3D{mx, my, mz},
                             Data3D{gx, gy, gz},
                             pressure_hPa,
                             temperature.fl,
                             air_speed);
}

/**
 * Builds an Event struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return An Event struct
 */
EventPacket *
PayloadDataConverter::toEventPacket(const FlyableType &flyableType, const uint32_t &sequenceNumber, const std::vector<uint8_t> &payloadBuffer) {
    assert(payloadBuffer.size() == PayloadType::EVENT.length());

    auto it = payloadBuffer.begin();

    auto measurement_time = parse32<uint32_t>(it);
    auto eventCode = parse8<uint8_t>(it);

    EventPacket r{};

    r.timestamp_ = measurement_time;

    if (RocketEventConstants::EVENT_CODES.find(eventCode) != RocketEventConstants::EVENT_CODES.end()) {
        r.code_ = eventCode;

    } else {
        r.code_ = RocketEventConstants::INVALID_EVENT_CODE;
    }

    return new EventPacket(r);
}

/**
 * Builds a Control struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return A Control struct
 */
ControlPacket *
PayloadDataConverter::toControlPacket(const FlyableType &flyableType, const uint32_t &sequenceNumber, const std::vector<uint8_t> &payloadBuffer) {
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

    return new ControlPacket(r);
}


/**
 * Builds a GPS struct given a sequence of bytes
 *
 * @param payloadBuffer The sequence of bytes from which to build the Telemetry struct
 * @return A GPS struct
 */
GPSPacket *
PayloadDataConverter::toGPSPacket(const FlyableType &flyableType, const uint32_t &sequenceNumber, const std::vector<uint8_t> &payloadBuffer) {
    assert(payloadBuffer.size() == PayloadType::GPS.length());

    auto it = payloadBuffer.begin();

    auto measurement_time = parse32<uint32_t>(it);

    auto satsCount = parse8<uint8_t>(it);

    float_cast hdop = {.uint32 = parse32<uint32_t>(it)};
    float_cast lat = {.uint32 = parse32<uint32_t>(it)};
    float_cast lon = {.uint32 = parse32<uint32_t>(it)};
    int32_t alt = parse32<int32_t>(it);

    return new GPSPacket(measurement_time,
                         sequenceNumber,
                         flyableType,
                         satsCount,
                         hdop.fl,
                         lat.fl,
                         lon.fl,
                         alt / 100.0f);
}


/**
 * Builds an ATCommandResponse struct given a sequence of bytes
 *
 * @return An ATCommandResponse struct
 */
ATCommandResponse *
PayloadDataConverter::toATCommandResponse(const std::vector<uint8_t> &payloadBuffer) {
    assert(payloadBuffer.size() == AT_RESPONSE_LENGTH);

    auto it = payloadBuffer.begin();

    auto frameType = parse8<uint8_t>(it);
    auto frameID = parse8<uint8_t>(it);
    auto command = parse16<uint16_t>(it);
    auto status = parse8<uint8_t>(it);
    auto response = parse8<uint8_t>(it);

    return new ATCommandResponse{
            frameType,
            frameID,
            command,
            status,
            response
    };
}