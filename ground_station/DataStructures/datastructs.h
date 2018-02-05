#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <utility>
#include "ILoggable.h"
#include "ProgramConstants.h"

using namespace std;
using namespace PrintConstants;

struct IDeserializable {
    virtual ~IDeserializable() = default;
};

struct TimedData {
    TimedData() = default;

    explicit TimedData(uint32_t timestamp) : timestamp_{timestamp} {}

    uint32_t timestamp_;
};

struct DataReading {
    DataReading() = default;

    DataReading(double v, bool b) : value{v}, validity{b} {}

    double value;
    bool validity;
};

struct RocketEvent : TimedData, ILoggable, IDeserializable {
    RocketEvent() = default;

    RocketEvent(const RocketEvent &that) = default;

    RocketEvent(uint32_t timestamp, int code, const std::string &description) :
            TimedData{timestamp}, code{code}, description{std::move(description)} {}

    ~RocketEvent() = default;

    int code;
    std::string description;

    string toString() const override {
        stringstream ss;

        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << timestamp_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << code
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << " "
           << description;

        return ss.str();
    }
};

struct ControlStatus : TimedData, IDeserializable {
    ControlStatus() = default;

    ControlStatus(const ControlStatus &that) = default;

    ControlStatus(uint32_t timestamp, uint8_t partCode, uint16_t statusValue) :
            TimedData{timestamp}, partCode_{partCode}, statusValue_{statusValue} {}

    uint8_t partCode_;
    uint16_t statusValue_;
};

struct XYZReading : ILoggable {
    XYZReading() : x_{0}, y_{0}, z_{0} {};

    XYZReading(double x, double y, double z) : x_{x}, y_{y}, z_{z} {}

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

    double norm() const{
        return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    }

    XYZReading operator+(XYZReading &rhs) const {
        XYZReading result{};
        result.x_ = this->x_ + rhs.x_;
        result.y_ = this->y_ + rhs.y_;
        result.z_ = this->z_ + rhs.z_;
        return result;
    }

    XYZReading operator*=(double coeff) {
        x_ *= coeff;
        y_ *= coeff;
        z_ *= coeff;

    }

    XYZReading operator+=(const XYZReading &rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;
    }
};

struct TelemetryReading : TimedData, ILoggable, IDeserializable {
    TelemetryReading() : TimedData{0},
                         altitude_{0},
                         acceleration_{},
                         magnetometer_{},
                         gyroscope_{},
                         pressure_{0},
                         temperature_{0},
                         air_speed_{0},
                         sequenceNumber_{0} {};

    TelemetryReading(uint32_t t, double altitude, XYZReading acceleration,
                     XYZReading magnetometer, XYZReading gyroscope,
                     double pressure, double temperature, double air_speed, uint32_t sequenceNumber) :
            TimedData{t},
            altitude_{altitude},
            acceleration_{acceleration},
            magnetometer_{magnetometer},
            gyroscope_{gyroscope},
            pressure_{pressure},
            temperature_{temperature},
            air_speed_{air_speed},
            sequenceNumber_{sequenceNumber} {}

    TelemetryReading(const TelemetryReading &that) = default;

    ~TelemetryReading() override = default;

    XYZReading acceleration_;
    XYZReading magnetometer_;
    XYZReading gyroscope_;
    double altitude_;
    double pressure_;
    double temperature_;
    double air_speed_;
    uint32_t sequenceNumber_;

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
           << magnetometer_.toString();

        return ss.str();
    }

    TelemetryReading operator+=(const TelemetryReading &rhs) {
        air_speed_ += rhs.air_speed_;
        pressure_ += rhs.pressure_;
        temperature_ += rhs.temperature_;
        altitude_ += rhs.altitude_;
        acceleration_ += rhs.acceleration_;
        magnetometer_ += rhs.magnetometer_;
        gyroscope_ += rhs.gyroscope_;
    }


    TelemetryReading operator*=(double coeff) {
        air_speed_ *= coeff;
        pressure_ *= coeff;
        temperature_ *= coeff;
        altitude_ *= coeff;
        acceleration_ *= coeff;
        magnetometer_ *= coeff;
        gyroscope_ *= coeff;
    }

};

#endif // DATASTRUCTS_H
