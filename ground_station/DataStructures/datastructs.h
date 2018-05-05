#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <utility>
#include <Flyable.h>
#include "ILoggable.h"
#include "ProgramConstants.h"

using namespace std;
using namespace PrintConstants;

struct IDeserializable {
    virtual ~IDeserializable() = default;

    virtual bool isValid() const = 0;
};

struct DataPacket {
    DataPacket() = default;

    explicit DataPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType) :
            timestamp_{timestamp},
            sequenceNumber_{sequenceNumber},
            flyableType_{flyableType} {}

    uint32_t timestamp_;
    uint32_t sequenceNumber_;
    FlyableType flyableType_;
};


struct EventPacket : DataPacket, ILoggable, IDeserializable {
    EventPacket() = default;

    EventPacket(const EventPacket &that) = default;

    EventPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                int code, const std::string &description) :
            DataPacket{timestamp, sequenceNumber, flyableType}, code_{code}, description_{std::move(description)} {}

    ~EventPacket() = default;

    int code_{};
    std::string description_;

    string toString() const override {
        stringstream ss;

        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << timestamp_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << code_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << " "
           << description_;

        return ss.str();
    }

    bool isValid() const override {
        return false;
    }
};

struct ControlPacket : DataPacket, IDeserializable {
    ControlPacket() = default;

    ControlPacket(const ControlPacket &that) = default;

    ControlPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                  uint8_t partCode, uint16_t statusValue) :
            DataPacket{timestamp, sequenceNumber, flyableType}, partCode_{partCode}, statusValue_{statusValue} {}

    uint8_t partCode_;
    uint16_t statusValue_;

    bool isValid() const override {
        return false;
    }
};

struct GPSPacket : DataPacket, ILoggable, IDeserializable {
    GPSPacket() = default;

    GPSPacket(const GPSPacket &that) = default;

    GPSPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
              uint8_t satsCount, float hdop, float latitude, float longitude, float altitude) :
            DataPacket{timestamp, sequenceNumber, flyableType},
            satsCount_{satsCount},
            hdop_{hdop},
            latitude_{latitude},
            longitude_{longitude},
            altitude_{altitude} {}

    uint8_t satsCount_;
    float hdop_;
    float latitude_;
    float longitude_;
    float altitude_;

    string toString() const override {
        stringstream ss;

        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << timestamp_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << (int) satsCount_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << hdop_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION_GPS) << fixed << latitude_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION_GPS) << fixed << longitude_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << altitude_;

        return ss.str();
    }

    bool isValid() const override {
        return (-90.0f <= latitude_ && latitude_ <= 90.0f)
               &
               (-180.0f <= longitude_ && longitude_ <= 180.0f);
    }
};

struct Data3D : ILoggable {
    Data3D() : x_{0}, y_{0}, z_{0} {};

    Data3D(double x, double y, double z) : x_{x}, y_{y}, z_{z} {}

    double x_;
    double y_;
    double z_;

    string toString() const override {
        stringstream ss;

        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << x_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << y_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << z_;

        return ss.str();
    }

    double norm() const {
        return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    }

    Data3D operator+(Data3D &rhs) const {
        Data3D result{};
        result.x_ = this->x_ + rhs.x_;
        result.y_ = this->y_ + rhs.y_;
        result.z_ = this->z_ + rhs.z_;
        return result;
    }

    Data3D operator*=(double coeff) {
        x_ *= coeff;
        y_ *= coeff;
        z_ *= coeff;

    }

    Data3D operator+=(const Data3D &rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;
    }
};

struct SensorsPacket : DataPacket, ILoggable, IDeserializable {
    SensorsPacket() : DataPacket{0, 0, FlyableType::ROCKET},
                      altitude_{0},
                      acceleration_{},
                      eulerAngles_{},
                      gyroscope_{},
                      pressure_{0},
                      temperature_{0},
                      air_speed_{0} {};

    SensorsPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                  double altitude, Data3D acceleration,
                  Data3D magnetometer, Data3D gyroscope,
                  double pressure, double temperature, double air_speed) :
            DataPacket{timestamp, sequenceNumber, flyableType},
            altitude_{altitude},
            acceleration_{acceleration},
            eulerAngles_{magnetometer},
            gyroscope_{gyroscope},
            pressure_{pressure},
            temperature_{temperature},
            air_speed_{air_speed} {}

    SensorsPacket(const SensorsPacket &that) = default;

    ~SensorsPacket() override = default;

    Data3D acceleration_;
    Data3D eulerAngles_;
    Data3D gyroscope_;
    double altitude_;
    double pressure_;
    double temperature_;
    double air_speed_;

    string toString() const override {
        stringstream format;
        format << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed;

        stringstream ss;
        ss << sequenceNumber_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << this->timestamp_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << altitude_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << air_speed_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << pressure_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << temperature_
           << acceleration_.toString()
           << gyroscope_.toString()
           << eulerAngles_.toString();

        return ss.str();
    }

    SensorsPacket operator+=(const SensorsPacket &rhs) {
        air_speed_ += rhs.air_speed_;
        pressure_ += rhs.pressure_;
        temperature_ += rhs.temperature_;
        altitude_ += rhs.altitude_;
        acceleration_ += rhs.acceleration_;
        eulerAngles_ += rhs.eulerAngles_;
        gyroscope_ += rhs.gyroscope_;
    }


    SensorsPacket operator*=(double coeff) {
        air_speed_ *= coeff;
        pressure_ *= coeff;
        temperature_ *= coeff;
        altitude_ *= coeff;
        acceleration_ *= coeff;
        eulerAngles_ *= coeff;
        gyroscope_ *= coeff;
    }

    bool isValid() const override {
        return false;
    }
};

#endif // DATASTRUCTS_H
